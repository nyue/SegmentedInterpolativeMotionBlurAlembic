#pragma once

#include "AbstractPointsSchemaHandler.h"

class RendermanPointsSchemaHandler : public AbstractPointsSchemaHandler
{
public:
	RendermanPointsSchemaHandler();
	virtual ~RendermanPointsSchemaHandler();
protected:
	virtual void EmitPoints(Alembic::AbcGeom::IPoints& points,
							Alembic::Abc::index_t      i_start_frame_number,
							Alembic::Abc::index_t      i_requested_frame_number,
							Alembic::Abc::uint8_t 	   i_motion_samples,
							float          			   i_relative_shutter_open,
							float          			   i_relative_shutter_close,
							const char*                i_renderer_filename) const;
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
