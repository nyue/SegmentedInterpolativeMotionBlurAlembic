#include "RendermanPolyMeshSchemaHandler.h"
#include "RendermanUtils.h"
#include "Utils.h"
#include <simba_logging.h>

RendermanPolyMeshSchemaHandler::RendermanPolyMeshSchemaHandler()
{

}

RendermanPolyMeshSchemaHandler::~RendermanPolyMeshSchemaHandler()
{

}

void RendermanPolyMeshSchemaHandler::EmitPolyMesh(Alembic::AbcGeom::IPolyMesh& pmesh,
												  Alembic::Abc::index_t        i_start_frame_number,
												  Alembic::Abc::index_t        i_requested_frame_number,
												  Alembic::Abc::uint8_t 	   i_motion_samples,
												  float          			   i_relative_shutter_open,
												  float          			   i_relative_shutter_close,
												  const char*                  i_renderer_filename) const
{

    size_t num_samples = pmesh.getSchema().getNumSamples();
    size_t last_sample_index = num_samples - 1;
    Alembic::Abc::int64_t requested_index = i_requested_frame_number - i_start_frame_number - 1;
    DLOG(INFO) << boost::format("num_samples = %1% i_requested_frame_number = %2% requested_index = %3% last_sample_index = %4%") % num_samples % i_requested_frame_number % requested_index % last_sample_index << std::endl;
    if (requested_index < 0 || requested_index > (num_samples-1))
    	return;

	Alembic::Abc::ISampleSelector current_sample_selector(requested_index);
    Alembic::AbcGeom::IPolyMeshSchema::Sample current_sample;
	pmesh.getSchema().get( current_sample, current_sample_selector );

    if (requested_index == 0)
    {
    	// First frame
    	Alembic::Abc::ISampleSelector next_sample_selector(requested_index+1);

        Alembic::AbcGeom::IPolyMeshSchema::Sample next_sample;
    	pmesh.getSchema().get( next_sample, next_sample_selector );

    	RendermanMeshData renderman_mesh_data;
    	build_polymesh_for_renderman_rib(0,
										 &current_sample,
										 &next_sample,
										 i_relative_shutter_open,
										 i_relative_shutter_close,
										 i_motion_samples,
										 renderman_mesh_data);
    	emit_renderman_mesh_data(renderman_mesh_data,
    							 i_relative_shutter_open,
								 i_relative_shutter_close);
    }
    else if (requested_index == last_sample_index)
    {
    	// Last frame
    	Alembic::Abc::ISampleSelector previous_sample_selector(requested_index-1);

        Alembic::AbcGeom::IPolyMeshSchema::Sample previous_sample;
    	pmesh.getSchema().get( previous_sample, previous_sample_selector );

    	RendermanMeshData renderman_mesh_data;
    	build_polymesh_for_renderman_rib(&previous_sample,
										 &current_sample,
										 0,
										 i_relative_shutter_open,
										 i_relative_shutter_close,
										 i_motion_samples,
										 renderman_mesh_data);
    	emit_renderman_mesh_data(renderman_mesh_data,
    							 i_relative_shutter_open,
								 i_relative_shutter_close);

    }
    else
    {
    	// Frame between first and last frame
    	Alembic::Abc::int64_t previous_index = requested_index - 1;
    	Alembic::Abc::int64_t next_index = requested_index + 1;
    	Alembic::Abc::ISampleSelector previous_sample_selector(previous_index);
    	Alembic::Abc::ISampleSelector next_sample_selector(next_index);
    	DLOG(INFO) << boost::format("previous_index = %1% requested_index = %2% next_index = %3%") % previous_index % requested_index % next_index << std::endl;
        Alembic::AbcGeom::IPolyMeshSchema::Sample next_sample;
    	pmesh.getSchema().get( next_sample, next_sample_selector );

        Alembic::AbcGeom::IPolyMeshSchema::Sample previous_sample;
    	pmesh.getSchema().get( previous_sample, previous_sample_selector );

    	RendermanMeshData renderman_mesh_data;
    	build_polymesh_for_renderman_rib(&previous_sample,
										 &current_sample,
										 &next_sample,
										 i_relative_shutter_open,
										 i_relative_shutter_close,
										 i_motion_samples,
										 renderman_mesh_data);
    	emit_renderman_mesh_data(renderman_mesh_data,
    							 i_relative_shutter_open,
								 i_relative_shutter_close);
    	write_renderman_mesh_data_to_wavefront_sequence(renderman_mesh_data,"mesh_per_sample.%04d.obj");
    }


}

void RendermanPolyMeshSchemaHandler::build_polymesh_for_renderman_rib(const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_previous_sample,
																	  const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_current_sample,
																	  const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_next_sample,
																	  float											   i_relative_shutter_open,
																	  float											   i_relative_shutter_close,
																	  Alembic::Abc::uint8_t                            i_motion_sample_count,
																	  RendermanMeshData&                               o_renderman_mesh) const
{
	if (!i_current_sample)
		return;

	// Assumes topologically stable
	Alembic::AbcGeom::Int32ArraySamplePtr face_indices = i_current_sample->getFaceIndices();
	Alembic::AbcGeom::Int32ArraySamplePtr face_counts = i_current_sample->getFaceCounts();

	// Current
	Alembic::AbcGeom::P3fArraySamplePtr current_P = i_current_sample->getPositions();
	Alembic::AbcGeom::V3fArraySamplePtr current_v = i_current_sample->getVelocities();
	size_t current_num_P = current_P->size();
	size_t current_num_v = current_v->size();

	// Topologically constant
	size_t num_nsides = face_counts->size();
	size_t num_indices = face_indices->size();

	/*!
	 * \remark Alembic does not currently support "holes", therefore, there is
	 *         only 1 loop, the visible loop.
	 */
	o_renderman_mesh._nloops_data.resize(num_nsides,1);
	o_renderman_mesh._nvertices_data.resize(num_nsides);
	o_renderman_mesh._vertices_data.resize(num_indices);
//	o_arnold_mesh._nsides_data.resize(num_nsides);
//	o_arnold_mesh._vidxs_data.resize(num_indices);
	for (size_t index=0;index<num_nsides;index++)
	{
		o_renderman_mesh._nvertices_data[index] = face_counts->get()[index];
	}
	for (size_t index=0;index<num_indices;index++)
	{
		o_renderman_mesh._vertices_data[index] = face_indices->get()[index];
	}

	if ( i_motion_sample_count == 1)
	{
		// Special case, return single time step from current sample
		DLOG(INFO) << boost::format("num_nsides=%1% num_indices=%2% num_P=%3%") % num_nsides % num_indices % current_num_P << std::endl;
		o_renderman_mesh._P_data_array.resize(boost::extents[i_motion_sample_count][current_num_P]);
		for (size_t index=0;index<current_num_P;index++)
		{
			o_renderman_mesh._P_data_array[0][index].x = current_P->get()[index].x;
			o_renderman_mesh._P_data_array[0][index].y = current_P->get()[index].y;
			o_renderman_mesh._P_data_array[0][index].z = current_P->get()[index].z;
		}

		return;
	}

	if (i_previous_sample && i_next_sample)
	{
		DLOG(INFO) << "INTERPOLATE INBETWEEN" << std::endl;

		Alembic::AbcGeom::P3fArraySamplePtr previous_P = i_previous_sample->getPositions();
		Alembic::AbcGeom::V3fArraySamplePtr previous_v = i_previous_sample->getVelocities();
		size_t previous_num_P = previous_P->size();
		size_t previous_num_v = previous_v->size();

		Alembic::AbcGeom::P3fArraySamplePtr next_P = i_next_sample->getPositions();
		Alembic::AbcGeom::V3fArraySamplePtr next_v = i_next_sample->getVelocities();
		size_t next_num_P = next_P->size();
		size_t next_num_v = next_v->size();

		if (	(previous_num_P==current_num_P)
				&&
				(next_num_P==current_num_P)
				&&
				(previous_num_v==current_num_v)
				&&
				(next_num_v==current_num_v)
				)
		{

			FloatContainer earlier_sampling_time_vector;
			FloatContainer later_sampling_time_vector;
			build_even_motion_relative_time_samples(i_relative_shutter_open,
													i_relative_shutter_close,
													i_motion_sample_count,
													earlier_sampling_time_vector,
													later_sampling_time_vector);

			o_renderman_mesh._P_data_array.resize(boost::extents[i_motion_sample_count][current_num_P]);
			assert(i_motion_sample_count==(earlier_sampling_time_vector.size()+later_sampling_time_vector.size()));
			FloatContainer::const_iterator tsIter;
			FloatContainer::const_iterator tsEIter;
			size_t sampling_index = 0; // shared over both earlier_sampling_time_vector and later_sampling_time_vector

			// Start with earlier time sample first so it is monotonically increasing

			// EARLIER
			tsIter = earlier_sampling_time_vector.begin();
			tsEIter = earlier_sampling_time_vector.end();
			for (;tsIter!=tsEIter;++tsIter)
			{

				for (size_t index=0;index<current_num_P;index++)
				{
					Imath::Vec3<float> P1(previous_P->get()[index].x,previous_P->get()[index].y,previous_P->get()[index].z);
					Imath::Vec3<float> T1(previous_v->get()[index].x,previous_v->get()[index].y,previous_v->get()[index].z);
					Imath::Vec3<float> P2(current_P->get()[index].x,current_P->get()[index].y,current_P->get()[index].z);
					Imath::Vec3<float> T2(current_v->get()[index].x,current_v->get()[index].y,current_v->get()[index].z);
					Imath::Vec3<float> P;
					float s = 1+(*tsIter);
					DLOG(INFO) << boost::format("EARLIER s = %1%") % s << std::endl;
					interpolate<float>(P1,T1,P2,T2,s,P);
					DLOG(INFO) << boost::format("Earlier [sampling_index = %1%] P1 = %2%, T1 = %3%, P2 = %4%, T2 = %5% [s = %6%] {P =%7%}") % sampling_index % P1 % T1 % P2 % T2 % s % P << std::endl;


					o_renderman_mesh._P_data_array[sampling_index][index].x = P.x;
					o_renderman_mesh._P_data_array[sampling_index][index].y = P.y;
					o_renderman_mesh._P_data_array[sampling_index][index].z = P.z;
				}

				sampling_index++;

			}

			DLOG(INFO) << "Earlier/Later =================================" << std::endl;

			// LATER
			tsIter = later_sampling_time_vector.begin();
			tsEIter = later_sampling_time_vector.end();
			for (;tsIter!=tsEIter;++tsIter)
			{
				for (size_t index=0;index<current_num_P;index++)
				{

					Imath::Vec3<float> P1(current_P->get()[index].x,current_P->get()[index].y,current_P->get()[index].z);
					Imath::Vec3<float> T1(current_v->get()[index].x,current_v->get()[index].y,current_v->get()[index].z);
					Imath::Vec3<float> P2(next_P->get()[index].x,next_P->get()[index].y,next_P->get()[index].z);
					Imath::Vec3<float> T2(next_v->get()[index].x,next_v->get()[index].y,next_v->get()[index].z);
					Imath::Vec3<float> P;
					float s = *tsIter;
					// DLOG(INFO) << boost::format("s = %1%") % s << std::endl;
					interpolate<float>(P1,T1,P2,T2,s,P);
					DLOG(INFO) << boost::format("Later [sampling_index = %1%] P1 = %2%, T1 = %3%, P2 = %4%, T2 = %5% [s = %6%] {P =%7%}") % sampling_index % P1 % T1 % P2 % T2 % s % P << std::endl;


					o_renderman_mesh._P_data_array[sampling_index][index].x = P.x;
					o_renderman_mesh._P_data_array[sampling_index][index].y = P.y;
					o_renderman_mesh._P_data_array[sampling_index][index].z = P.z;
				}

				sampling_index++;
			}
		}
	}
	else if ((i_previous_sample == 0) && i_next_sample)
	{
		DLOG(INFO) << "INTERPOLATE FIRST FRAME" << std::endl;
	}
	else if (i_previous_sample && (i_next_sample == 0))
	{
		DLOG(INFO) << "INTERPOLATE LAST FRAME" << std::endl;
	}


}

// == Emacs ================
// -------------------------
// Local variables:
// tab-width: 4
// indent-tabs-mode: t
// c-basic-offset: 4
// end:
//
// == vi ===================
// -------------------------
// Format block
// ex:ts=4:sw=4:expandtab
// -------------------------
