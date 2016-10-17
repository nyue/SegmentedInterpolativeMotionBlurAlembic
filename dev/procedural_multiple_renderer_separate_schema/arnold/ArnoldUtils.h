#pragma once

#include "ArnoldTypes.h"
#include <Alembic/AbcGeom/All.h>

// PolyMesh
struct AtNode* create_arnold_polymesh_node(const std::string&    name,
										  const ArnoldMeshData& i_arnold_mesh_data,
										  float                 i_shutter_open,
										  float                 i_shutter_close);

void emit_arnold_mesh_data(const ArnoldMeshData& i_arnold_mesh_data,
									float 				  i_shutter_open,
									float 				  i_shutter_close,
									 AtNodePtrContainer&     o_created_nodes,
									const char*           i_arnold_filename = 0);

void write_arnold_mesh_data_to_wavefront_file(const ArnoldMeshData&     i_arnold_mesh_data,
											  V3fSamplingArray2D::index i_sample_index,
											  const std::string&        i_wavefront_filename);

void write_arnold_mesh_data_to_wavefront_sequence(const ArnoldMeshData& i_arnold_mesh_data,
												  const std::string&    i_base_filename);

// Points

struct AtNode* create_arnold_points_node(const std::string&      name,
										const ArnoldPointsData& i_arnold_points_data,
										float                   i_shutter_open,
										float                   i_shutter_close);
void emit_arnold_points_data(const ArnoldPointsData& i_arnold_points_data,
							 float 				     i_shutter_open,
							 float 				     i_shutter_close,
							 AtNodePtrContainer&     o_created_nodes,
							 const char*             i_arnold_filename = 0);

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
