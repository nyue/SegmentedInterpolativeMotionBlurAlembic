#pragma once
#include <stdint.h>
#include <vector>
#include <map>
#include <boost/multi_array.hpp>
#include <OpenEXR/ImathVec.h>
#include <Alembic/Abc/All.h>

/*!
 * \TODO replace uint64_t with Alembic's definition so that it is cross platform
 */
typedef boost::multi_array<Imath::V3f, 2> V3fSamplingArray2D;
typedef std::vector<Alembic::Abc::uint64_t> UInt64Container;
typedef std::vector<std::string> StringContainer;
typedef std::vector<float> FloatContainer;

struct AlembicPointsData
{
	AlembicPointsData(const Imath::V3f& i_position, const Imath::V3f& i_velocity)
	: _position(i_position)
	, _velocity(i_velocity)
	{}
	Imath::V3f _position;
	Imath::V3f _velocity;
};
typedef std::map<Alembic::Abc::uint64_t,AlembicPointsData> AlembicPointsDataIndexedMap;

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
