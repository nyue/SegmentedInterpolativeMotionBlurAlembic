#include "RendermanPolyMeshSchemaHandler.h"
#include "RendermanUtils.h"

RendermanPolyMeshSchemaHandler::RendermanPolyMeshSchemaHandler()
{

}

RendermanPolyMeshSchemaHandler::~RendermanPolyMeshSchemaHandler()
{

}

void RendermanPolyMeshSchemaHandler::EmitPolyMesh(Alembic::AbcGeom::IPolyMesh& pmesh,
												  Alembic::Abc::index_t        i_start_frame_number,
												  Alembic::Abc::index_t        i_requested_frame_number,
												  const std::string&           i_arnold_filename,
												  Alembic::Abc::uint8_t 	   i_motion_samples,
												  float          			   i_relative_shutter_open,
												  float          			   i_relative_shutter_close) const
{

}

void RendermanPolyMeshSchemaHandler::build_polymesh_for_renderman_rib(const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_previous_sample,
																	  const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_current_sample,
																	  const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_next_sample,
																	  float											i_relative_shutter_open,
																	  float											i_relative_shutter_close,
																	  Alembic::Abc::uint8_t                            i_motion_sample_count,
																	  RendermanMeshData&                               o_renderman_mesh) const
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
