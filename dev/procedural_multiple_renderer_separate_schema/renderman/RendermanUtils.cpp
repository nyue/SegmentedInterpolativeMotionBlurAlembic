#include "RendermanUtils.h"
#include "Utils.h"
#include <glog/logging.h>

/*******************************************************************************
 * PolyMesh
 ******************************************************************************/
void create_renderman_polymesh_node(const RendermanMeshData& i_renderman_mesh_data,
									float                    i_shutter_open,
									float                    i_shutter_close)
{
	Alembic::Abc::uint8_t motion_sample_count = i_renderman_mesh_data._P_data_array.shape()[0];
	RtInt npolys = i_renderman_mesh_data._nvertices_data.size();
	bool has_multiple_samples = motion_sample_count > 1;


	if (has_multiple_samples)
	{
		FloatContainer       sampling_time_vector;
		build_single_even_motion_relative_time_samples(i_shutter_open,i_shutter_close,motion_sample_count,sampling_time_vector);
		RiMotionBeginV(sampling_time_vector.size(),sampling_time_vector.data());
	}
	for (Alembic::Abc::uint8_t motion_index=0;motion_index<motion_sample_count;motion_index++)
	{
		RiPointsGeneralPolygons(npolys,
								const_cast<RtInt *>(i_renderman_mesh_data._nloops_data.data()),
								const_cast<RtInt *>(i_renderman_mesh_data._nvertices_data.data()),
								const_cast<RtInt *>(i_renderman_mesh_data._vertices_data.data()),
								RI_P,i_renderman_mesh_data._P_data_array[motion_index].origin(),
								RI_NULL);
	}

	if (has_multiple_samples)
	{
		RiMotionEnd();
	}

}

void write_renderman_mesh_data_to_file(const RendermanMeshData& i_renderman_mesh_data,
									   const std::string&       i_renderman_filename,
									   float 				    i_shutter_open,
									   float 				    i_shutter_close)
{
	RiBegin(i_renderman_filename.c_str());

	create_renderman_polymesh_node(i_renderman_mesh_data,i_shutter_open,i_shutter_close);

	RiEnd();

}

void write_renderman_mesh_data_to_wavefront_file(const RendermanMeshData&  i_renderman_mesh_data,
												 V3fSamplingArray2D::index i_sample_index,
												 const std::string&        i_wavefront_filename)
{

}

void write_renderman_mesh_data_to_wavefront_sequence(const RendermanMeshData& i_renderman_mesh_data,
													 const std::string&       i_base_filename)
{

}

/*******************************************************************************
 * Points
 ******************************************************************************/

void create_renderman_points_node(const RendermanPointsData& i_renderman_points_data,
								  float                      i_shutter_open,
								  float                      i_shutter_close)
{
	Alembic::Abc::uint8_t motion_sample_count = i_renderman_points_data._P_data_array.shape()[0];
	Alembic::Abc::uint64_t points_count = i_renderman_points_data._P_data_array.shape()[1];
	DLOG(INFO) << boost::format("motion_sample_count = %1%") % int(motion_sample_count) << std::endl;
	DLOG(INFO) << boost::format("points_count = %1%") % int(points_count) << std::endl;
	bool has_multiple_samples = motion_sample_count > 1;
	RtInt npoints = i_renderman_points_data._ids_data.size();
	bool use_constantwidth = i_renderman_points_data._widths_data.size() != i_renderman_points_data._ids_data.size();
	if (has_multiple_samples)
	{
		FloatContainer       sampling_time_vector;
		build_single_even_motion_relative_time_samples(i_shutter_open,i_shutter_close,motion_sample_count,sampling_time_vector);
		RiMotionBeginV(sampling_time_vector.size(),sampling_time_vector.data());
	}

	if (use_constantwidth)
	{
		for (Alembic::Abc::uint8_t motion_index=0;motion_index<motion_sample_count;motion_index++)
		{
			RiPoints(points_count,RI_P,i_renderman_points_data._P_data_array[motion_index].origin(),
					 RI_CONSTANTWIDTH,i_renderman_points_data._widths_data.data(),
					 RI_NULL);
		}
	}
	else
	{
		for (Alembic::Abc::uint8_t motion_index=0;motion_index<motion_sample_count;motion_index++)
		{
			RiPoints(points_count,RI_P,i_renderman_points_data._P_data_array[motion_index].origin(),
					 RI_WIDTH,i_renderman_points_data._widths_data.data(),
					 RI_NULL);
		}

	}

	if (has_multiple_samples)
	{
		RiMotionEnd();
	}
}

void write_renderman_points_data_to_file(const RendermanPointsData& i_renderman_points_data,
										 const std::string&         i_renderman_filename,
										 float 				        i_shutter_open,
										 float 				        i_shutter_close)
{
	RiBegin(i_renderman_filename.c_str());

	create_renderman_points_node(i_renderman_points_data,i_shutter_open,i_shutter_close);

	RiEnd();
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
