#include "RendermanUtils.h"
#include "Utils.h"

/*******************************************************************************
 * PolyMesh
 ******************************************************************************/
void create_renderman_polymesh_node(const std::string&       name,
									const RendermanMeshData& i_renderman_mesh_data,
									float                    i_shutter_open,
									float                    i_shutter_close)
{

}

void write_renderman_mesh_data_to_file(const RendermanMeshData& i_renderman_mesh_data,
									   const std::string&       i_renderman_filename,
									   float 				    i_shutter_open,
									   float 				    i_shutter_close)
{

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

void build_points_for_renderman_rib_from_interim_points(const AlembicPointsDataIndexedMap* i_previous_interim_points,
														const AlembicPointsDataIndexedMap* i_current_interim_points,
														const AlembicPointsDataIndexedMap* i_next_interim_points,
														float							   i_relative_shutter_open,
														float							   i_relative_shutter_close,
														Alembic::Abc::uint8_t              i_motion_sample_count,
														RendermanPointsData&			   o_renderman_points)
{

}

void create_renderman_points_node(const std::string&         name,
								  const RendermanPointsData& i_renderman_points_data,
								  float                      i_shutter_open,
								  float                      i_shutter_close)
{
	Alembic::Abc::uint8_t motion_sample_count = i_renderman_points_data._P_data_array.shape()[0];
	bool has_multiple_samples = motion_sample_count > 1;
	RtInt npoints = i_renderman_points_data._ids_data.size();
	bool use_constantwidth = i_renderman_points_data._width_data.size() != i_renderman_points_data._ids_data.size();
	if (has_multiple_samples)
	{
		FloatContainer       sampling_time_vector;
		build_single_even_motion_relative_time_samples(i_shutter_open,i_shutter_close,motion_sample_count,sampling_time_vector);
		RiMotionBeginV(sampling_time_vector.size(),sampling_time_vector.data());
	}

	if (use_constantwidth)
	{
	}
	else
	{

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
