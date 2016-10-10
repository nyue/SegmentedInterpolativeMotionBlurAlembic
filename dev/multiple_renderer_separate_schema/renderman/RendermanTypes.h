#pragma once

#include <ri.h>
#include <vector>
#include "Types.h"

typedef std::vector<RtInt> RtIntContainer;
typedef std::vector<RtFloat> RtFloatContainer;

/*!
 * \remark Alembic does not support "holes" but to be safe, we are targeting
 *         RiPointsGeneralPolygons to future proof our code
 *
 *         RtVoid RiPointsGeneralPolygons(RtInt npolys, RtInt nloops[],
 *                                        RtInt nvertices[],
 *                                        RtInt vertices[], ...);
 *
 */
struct RendermanMeshData
{
	V3fSamplingArray2D _P_data_array;
	// Assumes topological stability
	RtInt _npolys;
	RtIntContainer _nloops_data;
	RtIntContainer _nvertices_data;
	RtIntContainer _vertices_data;
};

struct RendermanPointsData
{
	V3fSamplingArray2D _P_data_array;
	// Assumes topological stability
	RtIntContainer _ids_data;
	RtFloatContainer _width_data;
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
