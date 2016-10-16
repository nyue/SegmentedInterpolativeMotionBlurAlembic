#pragma once
#include "ArnoldTypes.h"
#include <ai.h>

/*!
 * \brief Serves as a base class to house the procedural's AtNode*
 *        container that needs to be shared by all subclass instances
 */
class ArnoldGeomSchemaHandler
{
public:
	ArnoldGeomSchemaHandler() {}
	virtual ~ArnoldGeomSchemaHandler() {}
	void set_nodes(const AtNodePtrContainerSharedPtr & i_nodes) { _nodes = i_nodes; }
	AtNodePtrContainerSharedPtr get_nodes() { return _nodes; }
	const AtNodePtrContainerSharedPtr get_nodes() const { return _nodes; }
protected:
	AtNodePtrContainerSharedPtr _nodes;
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
