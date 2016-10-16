#pragma once

#include "RendermanTypes.h"
#include <Alembic/AbcGeom/All.h>

// PolyMesh
void create_renderman_polymesh_node(const RendermanMeshData& i_renderman_mesh_data,
									float                    i_shutter_open,
									float                    i_shutter_close);

void write_renderman_mesh_data_to_file(const RendermanMeshData& i_renderman_mesh_data,
									   const char*              i_renderman_filename,
									   float 				    i_shutter_open,
									   float 				    i_shutter_close);

void write_renderman_mesh_data_to_wavefront_file(const RendermanMeshData&  i_renderman_mesh_data,
												 V3fSamplingArray2D::index i_sample_index,
												 const std::string&        i_wavefront_filename);

void write_renderman_mesh_data_to_wavefront_sequence(const RendermanMeshData& i_renderman_mesh_data,
													 const std::string&       i_base_filename);

// Points
//void build_points_for_renderman_rib_from_interim_points(const AlembicPointsDataIndexedMap* i_previous_interim_points,
//														const AlembicPointsDataIndexedMap* i_current_interim_points,
//														const AlembicPointsDataIndexedMap* i_next_interim_points,
//														float							   i_relative_shutter_open,
//														float							   i_relative_shutter_close,
//														Alembic::Abc::uint8_t              i_motion_sample_count,
//														RendermanPointsData&			   o_renderman_points);
void create_renderman_points_node(const RendermanPointsData& i_renderman_points_data,
								  float                      i_shutter_open,
								  float                      i_shutter_close);
void write_renderman_points_data_to_file(const RendermanPointsData& i_renderman_points_data,
		 	 	 	 	 	 	 	 	 const char*                i_renderman_filename,
										 float 				        i_shutter_open,
										 float 				        i_shutter_close);
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
