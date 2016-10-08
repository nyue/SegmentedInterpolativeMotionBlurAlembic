#pragma once

#include "AbstractGeomSchemaHandler.h"

class AbstractPointsSchemaHandler : public AbstractGeomSchemaHandler
{
public:
	AbstractPointsSchemaHandler();
	virtual ~AbstractPointsSchemaHandler();
	void ProcessPoints(const Alembic::Abc::IObject& top,
					   const std::string&           child_name,
					   const StringContainer&       i_hierachy_path,
					   size_t 					    i_requested_index,
					   float          			    i_relative_shutter_open,
					   float          			    i_relative_shutter_close,
					   size_t						num_motion_samples) const;
protected:
	virtual void EmitPoints(Alembic::AbcGeom::IPoints& pmesh,
	   	   	   	   	  Alembic::Abc::index_t        i_start_frame_number,
			  Alembic::Abc::index_t        i_requested_frame_number,
			  const std::string&           i_arnold_filename,
			  size_t 					   i_motion_samples,
			  float          			   i_relative_shutter_open,
			  float          			   i_relative_shutter_close) const = 0;
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
