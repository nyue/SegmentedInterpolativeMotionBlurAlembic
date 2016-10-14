#pragma once

#include <fstream>
#include <vector>
#include <map>
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

#include <OpenEXR/ImathVec.h>

#define EXTERNAL 1

class VelocitySideCar
{
public:
	typedef std::vector<Imath::V3f> V3fContainer;
	typedef std::map<std::string,int> NamedV3fContainerMap;
	int _value;
	std::string _source_md5;
	V3fContainer _v;
	NamedV3fContainerMap _velocities;
#ifndef EXTERNAL
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, Imath::V3f & s, const unsigned int version)
	{
		ar & s.x;
		ar & s.y;
		ar & s.z;
	}
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		// Simply list all the fields to be serialized/deserialized.
		ar & _value;
		ar & _source_md5;
		ar & _v;
//		ar & _velocities;    // It works for containers as well!
	}
#endif // EXTERNAL
public:
	VelocitySideCar(int i_value = 0, const std::string i_md5 = "")
	: _value(i_value)
	, _source_md5(i_md5){};
	virtual ~VelocitySideCar() {};
	int get_value() const { return _value; };
	const std::string& get_md5() const { return _source_md5; };
};

#ifdef EXTERNAL
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
void serialize(Archive & ar, VelocitySideCar::V3fContainer & s, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(s);
}

template<class Archive>
void serialize(Archive & ar, VelocitySideCar::NamedV3fContainerMap & s, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(s);
}

template<class Archive>
void serialize(Archive & ar, VelocitySideCar & s, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(s._velocities);
	ar & s._value;
	ar & s._source_md5;
}

} // namespace serialization
} // namespace boost
#endif // EXTERNAL

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
