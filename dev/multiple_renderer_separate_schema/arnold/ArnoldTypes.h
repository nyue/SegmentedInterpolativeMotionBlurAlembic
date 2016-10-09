#pragma once

#include <ai.h>
#include <vector>
#include "Types.h"

typedef std::vector<AtUInt32> AtUInt32Container;
typedef std::vector<AtUInt64> AtUInt64Container;

struct ArnoldMeshData
{
	V3fSamplingArray2D _vlist_data_array;
	// Topological stable being assumed
	AtUInt32Container _nsides_data;
	AtUInt32Container _vidxs_data;
};
struct ArnoldPointsData
{
	V3fSamplingArray2D _points_data_array;
	// Topological stable being assumed
	AtUInt64Container _ids_data;
	FloatContainer _radius_data;
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
