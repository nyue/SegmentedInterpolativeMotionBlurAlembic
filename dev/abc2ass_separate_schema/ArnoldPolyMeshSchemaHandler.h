#pragma once

#include "AbstractPolyMeshSchemaHandler.h"

class ArnoldPolyMeshSchemaHandler : public AbstractPolyMeshSchemaHandler
{
public:
	ArnoldPolyMeshSchemaHandler();
	virtual ~ArnoldPolyMeshSchemaHandler();
	void process(
			const Alembic::Abc::IObject& top,
			const std::string& child_name,
			  const StringContainer&       i_hierachy_path

			) const;
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
