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
	ArnoldGeomSchemaHandler() : _nodes(0){}
	virtual ~ArnoldGeomSchemaHandler() {}
	void set_created_nodes_container(AtNodePtrContainer* i_nodes) { _nodes = i_nodes; }
	AtNodePtrContainer* get_created_nodes_container() { return _nodes; }
protected:
	AtNodePtrContainer* _nodes;
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
