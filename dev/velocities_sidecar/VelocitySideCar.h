#pragma once

#include <fstream>
#include <vector>
#include <map>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <OpenEXR/ImathVec.h>

class VelocitySideCar
{
public:
	typedef std::vector<Imath::V3f> V3fContainer;
	typedef std::map<std::string,V3fContainer> NamedV3fContainerMap;
//	int _value;
	std::string _source_md5;
	NamedV3fContainerMap _velocities;
};

namespace boost {
	namespace serialization {

	template<class Archive>
	void serialize(Archive & ar, Imath::V3f & s, const unsigned int version)
	{
		ar & s.x;
		ar & s.y;
		ar & s.z;
	}

	template<class Archive>
	void serialize(Archive & ar, VelocitySideCar & s, const unsigned int version)
	{
		ar & s._source_md5;
		ar & BOOST_SERIALIZATION_NVP(s._velocities);
	}

	} // namespace serialization
} // namespace boost

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
