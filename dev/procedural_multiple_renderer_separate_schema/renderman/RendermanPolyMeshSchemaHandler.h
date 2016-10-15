#pragma once

#include "AbstractPolyMeshSchemaHandler.h"
#include "RendermanTypes.h"

class RendermanPolyMeshSchemaHandler : public AbstractPolyMeshSchemaHandler
{

public:
	RendermanPolyMeshSchemaHandler();
	virtual ~RendermanPolyMeshSchemaHandler();
protected:
	virtual void EmitPolyMesh(Alembic::AbcGeom::IPolyMesh& pmesh,
							  Alembic::Abc::index_t        i_start_frame_number,
							  Alembic::Abc::index_t        i_requested_frame_number,
							  Alembic::Abc::uint8_t 	   i_motion_samples,
							  float          			   i_relative_shutter_open,
							  float          			   i_relative_shutter_close,
							  const char*                  i_renderer_filename) const;
private:
	void build_polymesh_for_renderman_rib(const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_previous_sample,
										  const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_current_sample,
										  const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_next_sample,
										  float											i_relative_shutter_open,
										  float											i_relative_shutter_close,
										  Alembic::Abc::uint8_t                            i_motion_sample_count,
										  RendermanMeshData&                               o_renderman_mesh) const;
};
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
