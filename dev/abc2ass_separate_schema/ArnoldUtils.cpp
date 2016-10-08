#include "ArnoldUtils.h"
#include "Utils.h"
#include <boost/format.hpp>
#include <fstream>

// =============================================================================
// Arnold PolyMesh
// =============================================================================

void create_arnold_polymesh_node(const std::string&    name,
								 const ArnoldMeshData& i_arnold_mesh_data,
								 float                 i_shutter_open,
								 float                 i_shutter_close)
{
	AtByte nkeys = i_arnold_mesh_data._vlist_data_array.size();
	if (nkeys<1)
		return;
	// Assumes topological stability
	AtUInt32 nelements = i_arnold_mesh_data._vlist_data_array[0].size();
	// name
	AtNode *polymesh = AiNode("polymesh");
	AiNodeSetStr(polymesh, "name", name.c_str());

	std::cout << boost::format("nelements=%1% nkeys=%2%") % nelements % int(nkeys)<< std::endl;

	// vlist
	AiNodeSetArray(polymesh, "vlist", AiArrayConvert(nelements,nkeys,AI_TYPE_POINT,i_arnold_mesh_data._vlist_data_array.data()));

	// nsides
	AiNodeSetArray(polymesh, "nsides", AiArrayConvert(i_arnold_mesh_data._nsides_data.size(),1,AI_TYPE_UINT,i_arnold_mesh_data._nsides_data.data()));

	// vidxs
	AiNodeSetArray(polymesh, "vidxs", AiArrayConvert(i_arnold_mesh_data._vidxs_data.size(),1,AI_TYPE_UINT,i_arnold_mesh_data._vidxs_data.data()));

	// deform_time_samples
	float deform_time_samples[2] = {i_shutter_open,i_shutter_close};
	AiNodeSetArray(polymesh, "deform_time_samples", AiArrayConvert(2,1,AI_TYPE_FLOAT,deform_time_samples));

}

void write_arnold_mesh_data_to_file(const ArnoldMeshData& i_arnold_mesh_data,
									const std::string&    i_arnold_filename,
									float 				  i_shutter_open,
									float 				  i_shutter_close)
{
	// start an Arnold session
	AiBegin();

	create_arnold_polymesh_node("test",i_arnold_mesh_data,i_shutter_open,i_shutter_close);

	int write_mask = AI_NODE_ALL;
	bool is_open_procs = false;
	bool is_binary = false;
	// Write out an .ass file
	AiASSWrite(i_arnold_filename.c_str(), write_mask, is_open_procs, is_binary);

	// at this point we can shut down Arnold
	AiEnd();

}

void write_arnold_mesh_data_to_wavefront_file(const ArnoldMeshData&     i_arnold_mesh_data,
											  V3fSamplingArray2D::index i_sample_index,
											  const std::string&        i_wavefront_filename)
{

	std::ofstream wavefront_file;
	wavefront_file.open (i_wavefront_filename.c_str());

	wavefront_file << "# File exported by Nicholas Yue\n";


    size_t P_size = i_arnold_mesh_data._vlist_data_array[i_sample_index].size();
    size_t indices_size = i_arnold_mesh_data._vidxs_data.size();
    size_t counts_size = i_arnold_mesh_data._nsides_data.size();

    wavefront_file << boost::format("# %1% points") % P_size << std::endl;
    wavefront_file << boost::format("# %1% faces") % counts_size << std::endl;

    wavefront_file << "g" << std::endl;
    for (size_t index=0;index<P_size;index++)
    {
    	float x = i_arnold_mesh_data._vlist_data_array[i_sample_index][index].x;
    	float y = i_arnold_mesh_data._vlist_data_array[i_sample_index][index].y;
    	float z = i_arnold_mesh_data._vlist_data_array[i_sample_index][index].z;
        wavefront_file << boost::format("v %1% %2% %3%") % x % y % z << std::endl;
    }

    wavefront_file << "g" << std::endl;
    size_t indices_index = 0;
    for (size_t index=0;index<counts_size;index++)
    {
        wavefront_file << "f";
    	size_t face_vert_count = i_arnold_mesh_data._nsides_data[index];
    	for (size_t face_vert_index=0;face_vert_index < face_vert_count; ++face_vert_index)
    	{
    		size_t face_vert_index_value = i_arnold_mesh_data._vidxs_data[indices_index] + 1; // OBJ array is not zero based!
    		wavefront_file << boost::format(" %1%") % face_vert_index_value;
    		indices_index++;
    	}
		wavefront_file << std::endl;
    }
    wavefront_file.close();
}

void write_arnold_mesh_data_to_wavefront_sequence(const ArnoldMeshData& i_arnold_mesh_data,
												  const std::string&    i_base_filename)
{

	V3fSamplingArray2D::index num_samples = i_arnold_mesh_data._vlist_data_array.size();
	V3fSamplingArray2D::index num_elements_per_sample = i_arnold_mesh_data._vlist_data_array.shape()[1];
	std::cout << boost::format("num_elements_per_sample = %1%") % num_elements_per_sample << std::endl;
	for (V3fSamplingArray2D::index sample_index=0;sample_index<num_samples;++sample_index)
	{
		std::string numbered_output_filename = (boost::format(i_base_filename) % (sample_index+1)).str();
		std::cout << boost::format("numbered_output_filename = '%1%'") % numbered_output_filename << std::endl;
		write_arnold_mesh_data_to_wavefront_file(i_arnold_mesh_data,sample_index,numbered_output_filename);
	}
	std::cout << boost::format("_vlist_data_array num_elements = %1%") % i_arnold_mesh_data._vlist_data_array.size() << std::endl;
}

// =============================================================================
// Arnold Points
// =============================================================================


void build_interim_points_for_arnold_ass(const Alembic::AbcGeom::IPointsSchema::Sample* i_sample,
										 AlembicPointsDataIndexedMap&                   o_interim_points)
{

	Alembic::AbcGeom::P3fArraySamplePtr positions = i_sample->getPositions();
	Alembic::AbcGeom::V3fArraySamplePtr velocities = i_sample->getVelocities();
	Alembic::AbcGeom::UInt64ArraySamplePtr ids = i_sample->getIds();
	size_t num_positions = positions->size();
	size_t num_velocities = velocities->size();
	size_t num_ids = ids->size();
	assert ( num_positions == num_ids );
	assert ( num_velocities == num_ids );
	std::pair<AlembicPointsDataIndexedMap::iterator,bool> ret;
	for (size_t positions_index = 0;positions_index < num_positions; positions_index++)
	{
		ret = o_interim_points.insert(AlembicPointsDataIndexedMap::value_type(
				ids->get()[positions_index],
				AlembicPointsData(Imath::V3f(positions->get()[positions_index].x,positions->get()[positions_index].y,positions->get()[positions_index].z),
						Imath::V3f(velocities->get()[positions_index].x,velocities->get()[positions_index].y,velocities->get()[positions_index].z))));
		if (!ret.second)
		{
			std::cerr << "Failed to insert into map" << std::endl;
			return;
		}
	}
	// std::cout << boost::format("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX o_interim_points = %1%") % o_interim_points.size() << std::endl;
}

void build_points_for_arnold_ass_from_interim_points(const AlembicPointsDataIndexedMap* i_previous_interim_points,
													 const AlembicPointsDataIndexedMap* i_current_interim_points,
													 const AlembicPointsDataIndexedMap* i_next_interim_points,
													 float								i_relative_shutter_open,
													 float								i_relative_shutter_close,
													 AtByte								i_motion_sample_count,
													 ArnoldPointsData&					o_arnold_points)
{

	// Use the current interim point's id as lookup for the previous and next information
//	V3fSamplingArray2D::index num_test_samples = i_motion_sample_count;
//	std::cout << boost::format("YYYYYYYYYYYYYYYYYYYYYY num_test_samples = %1%") % num_test_samples << std::endl;
//	num_test_samples = 1;

	FloatContainer earlier_sampling_time_vector;
	FloatContainer later_sampling_time_vector;


	if (build_even_motion_relative_time_samples(i_relative_shutter_open,
												i_relative_shutter_close,
												i_motion_sample_count,
												earlier_sampling_time_vector,
												later_sampling_time_vector))
	{
		{
			size_t num_earlier_samples = earlier_sampling_time_vector.size();
			for (size_t index=0;index<num_earlier_samples;++index)
			{
				std::cout << boost::format("earlier_sampling_time_vector[%1%] = %2%") % index % earlier_sampling_time_vector[index] << std::endl;
			}
		}
		{
			size_t num_later_samples = later_sampling_time_vector.size();
			for (size_t index=0;index<num_later_samples;++index)
			{
				std::cout << boost::format("later_sampling_time_vector[%1%] = %2%") % index % later_sampling_time_vector[index] << std::endl;
			}
		}
	}
	o_arnold_points._points_data_array.resize(boost::extents[i_motion_sample_count][i_current_interim_points->size()]);
	V3fSamplingArray2D::index point_index = 0;
	for(AlembicPointsDataIndexedMap::const_iterator iter = i_current_interim_points->begin(); iter != i_current_interim_points->end(); iter++,point_index++)
	{
		uint64_t search_id = iter->first;
		o_arnold_points._ids_data.push_back(search_id);
		o_arnold_points._radius_data.push_back(0.01f);
		bool previous_point_exists = false;
		bool next_point_exists = false;
		// Generate samples for this particular point
		typedef std::vector<Imath::V3f> V3fContainer;

		V3fContainer interpolated_P(i_motion_sample_count);
		AlembicPointsDataIndexedMap::const_iterator find_previous_point_result;
		AlembicPointsDataIndexedMap::const_iterator find_next_point_result;
		if (i_previous_interim_points)
		{
			find_previous_point_result = i_previous_interim_points->find(search_id);
	        if (find_previous_point_result != i_previous_interim_points->end())
	        {
	        	previous_point_exists = true;
	        }
		}

		if (i_next_interim_points)
		{
			find_next_point_result = i_next_interim_points->find(search_id);
	        if (find_next_point_result != i_next_interim_points->end())
	        {
	        	next_point_exists = true;
	        }
		}

		// The are 3 possibilities - start, in-between and end
		if (!previous_point_exists && next_point_exists)
		{
			// START frame
			std::cout << "START frame : points processing" << std::endl;
		}
		else if (previous_point_exists && !next_point_exists)
		{
			// END frame
			std::cout << "END frame : points processing" << std::endl;
		}
		else if (previous_point_exists && next_point_exists)
		{
			// INBETWEEN frame
			std::cout << "INBETWEEN frame : points processing" << std::endl;
			size_t interpolated_P_index = 0;
			size_t num_earlier_sampling_time = earlier_sampling_time_vector.size();
			size_t num_later_sampling_time = later_sampling_time_vector.size();

			Imath::V3f P1;
			Imath::V3f T1;
			Imath::V3f P2;
			Imath::V3f T2;

			// Previous
			P1 = find_previous_point_result->second._position;
			T1 = find_previous_point_result->second._velocity;
			P2 = iter->second._position;
			T2 = iter->second._velocity;
			for (size_t earlier_sample_index=0;earlier_sample_index<num_earlier_sampling_time;++earlier_sample_index)
			{
				float s = 1+earlier_sampling_time_vector[earlier_sample_index];
				interpolate<float>(P1,T1,P2,T2,s,interpolated_P[interpolated_P_index]);
				interpolated_P_index++;
			}

			// Next
			P1 = iter->second._position;
			T1 = iter->second._velocity;
			P2 = find_next_point_result->second._position;
			T2 = find_next_point_result->second._velocity;
			for (size_t next_sample_index=0;next_sample_index<num_later_sampling_time;++next_sample_index)
			{
				float s = later_sampling_time_vector[next_sample_index];
				interpolate<float>(P1,T1,P2,T2,s,interpolated_P[interpolated_P_index]);
				interpolated_P_index++;
			}

			assert ( interpolated_P_index == i_motion_sample_count );
		}
		for (V3fSamplingArray2D::index motion_index = 0;motion_index<i_motion_sample_count;++motion_index)
		{
			o_arnold_points._points_data_array[motion_index][point_index] = interpolated_P[motion_index];
		}
	}

}

void create_arnold_points_node(const std::string&    name,
								 const ArnoldPointsData& i_arnold_points_data,
								 float                 i_shutter_open,
								 float                 i_shutter_close)
{
	AtByte nkeys = i_arnold_points_data._points_data_array.size();
	if (nkeys<1)
		return;
	// Assumes topological stability
	AtUInt32 nelements = i_arnold_points_data._points_data_array[0].size();
	// name
	AtNode *points = AiNode("points");
	AiNodeSetStr(points, "name", name.c_str());

	std::cout << boost::format("nelements=%1% nkeys=%2%") % nelements % int(nkeys)<< std::endl;

	// points
	AiNodeSetArray(points, "points", AiArrayConvert(nelements,nkeys,AI_TYPE_POINT,i_arnold_points_data._points_data_array.data()));

	// radius
	AiNodeSetArray(points, "radius", AiArrayConvert(i_arnold_points_data._radius_data.size(),1,AI_TYPE_FLOAT,i_arnold_points_data._radius_data.data()));

	// deform_time_samples
	float deform_time_samples[2] = {i_shutter_open,i_shutter_close};
	AiNodeSetArray(points, "deform_time_samples", AiArrayConvert(2,1,AI_TYPE_FLOAT,deform_time_samples));
}

void write_arnold_points_data_to_file(const ArnoldPointsData& i_arnold_points_data,
									  const std::string&      i_arnold_filename,
									  float 				  i_shutter_open,
									  float 				  i_shutter_close)
{
	// start an Arnold session
	AiBegin();
//	AiMsgSetLogFileName("pm.log");

	create_arnold_points_node("test",i_arnold_points_data,i_shutter_open,i_shutter_close);

	// ... or you can write out an .ass file instead
	AiASSWrite(i_arnold_filename.c_str(), AI_NODE_ALL, false, false);

	// at this point we can shut down Arnold
	AiEnd();


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
