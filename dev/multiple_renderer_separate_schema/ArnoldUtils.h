#pragma once

#include "ArnoldTypes.h"
#include <Alembic/AbcGeom/All.h>

// PolyMesh
void create_arnold_polymesh_node(const std::string&    name,
								 const ArnoldMeshData& i_arnold_mesh_data,
								 float                 i_shutter_open,
								 float                 i_shutter_close);

void write_arnold_mesh_data_to_file(const ArnoldMeshData& i_arnold_mesh_data,
									const std::string&    i_arnold_filename,
									float 				  i_shutter_open,
									float 				  i_shutter_close);

void write_arnold_mesh_data_to_wavefront_file(const ArnoldMeshData&     i_arnold_mesh_data,
											  V3fSamplingArray2D::index i_sample_index,
											  const std::string&        i_wavefront_filename);

void write_arnold_mesh_data_to_wavefront_sequence(const ArnoldMeshData& i_arnold_mesh_data,
												  const std::string&    i_base_filename);

// Points
void build_interim_points_for_arnold_ass(const Alembic::AbcGeom::IPointsSchema::Sample* i_sample,
										 AlembicPointsDataIndexedMap&                   o_interim_points);

void build_points_for_arnold_ass_from_interim_points(const AlembicPointsDataIndexedMap* i_previous_interim_points,
													 const AlembicPointsDataIndexedMap* i_current_interim_points,
													 const AlembicPointsDataIndexedMap* i_next_interim_points,
													 float								i_relative_shutter_open,
													 float								i_relative_shutter_close,
													 Alembic::Abc::uint8_t              i_motion_sample_count,
													 ArnoldPointsData&					o_arnold_points);
void create_arnold_points_node(const std::string&    name,
								 const ArnoldPointsData& i_arnold_points_data,
								 float                 i_shutter_open,
								 float                 i_shutter_close);
void write_arnold_points_data_to_file(const ArnoldPointsData& i_arnold_points_data,
									  const std::string&      i_arnold_filename,
									  float 				  i_shutter_open,
									  float 				  i_shutter_close);

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
