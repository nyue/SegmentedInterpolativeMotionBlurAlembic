#pragma once

#include <Alembic/Util/All.h>
#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCollection/All.h>
#include <Alembic/AbcMaterial/All.h>

#include <vector>
#include <boost/multi_array.hpp>
#include <OpenEXR/ImathVec.h>

typedef boost::multi_array<Imath::V3f, 2> V3fSamplingArray2D;
typedef std::vector<uint64_t> UInt64Container;
typedef std::vector<std::string> StringContainer;
typedef std::vector<float> FloatContainer;

class AbstractGeomSchemaHandler
{
public:
	AbstractGeomSchemaHandler();
	virtual ~AbstractGeomSchemaHandler();
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
