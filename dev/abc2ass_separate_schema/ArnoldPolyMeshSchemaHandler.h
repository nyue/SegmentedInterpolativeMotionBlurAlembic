#pragma once

#include "AbstractPolyMeshSchemaHandler.h"
#include "ArnoldTypes.h"

class ArnoldPolyMeshSchemaHandler : public AbstractPolyMeshSchemaHandler
{

public:
	ArnoldPolyMeshSchemaHandler();
	virtual ~ArnoldPolyMeshSchemaHandler();
protected:
	virtual void EmitPolyMesh(Alembic::AbcGeom::IPolyMesh& pmesh,
  	   	   	   Alembic::Abc::index_t        i_start_frame_number,
		   Alembic::Abc::index_t        i_requested_frame_number,
		   const std::string&           i_arnold_filename,
		   size_t 						i_motion_samples,
		   float          				i_relative_shutter_open,
		   float          				i_relative_shutter_close) const;
private:
	void build_polymesh_for_arnold_ass(const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_previous_sample,
									   const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_current_sample,
									   const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_next_sample,
									   float											i_relative_shutter_open,
									   float											i_relative_shutter_close,
									   size_t											i_motion_sample_count,
									   ArnoldMeshData&                                  o_arnold_mesh) const;
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
