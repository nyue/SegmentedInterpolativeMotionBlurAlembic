#pragma once

#include "AbstractGeomSchemaHandler.h"

class AbstractPointsSchemaHandler : public AbstractGeomSchemaHandler
{
public:
	AbstractPointsSchemaHandler();
	virtual ~AbstractPointsSchemaHandler();
protected:
	virtual void EmitPoints() const = 0;
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
