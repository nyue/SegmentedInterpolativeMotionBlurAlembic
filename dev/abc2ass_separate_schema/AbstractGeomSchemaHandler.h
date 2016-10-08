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
#include <boost/format.hpp>
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
protected:
	void flatten_string_array(const StringContainer& i_string_array,
		  const std::string&     i_separator,
		  std::string&           o_flatten_string) const;
	bool build_even_motion_relative_time_samples(float           i_relative_shutter_open,
												 float           i_relative_shutter_close,
												 size_t          i_motion_sample_count,
												 FloatContainer& o_earlier_sampling_time_vector,
												 FloatContainer& o_later_sampling_time_vector) const;
	template <typename T> void interpolate(const Imath::Vec3<T>& P1,
										   const Imath::Vec3<T>& T1,
										   const Imath::Vec3<T>& P2,
										   const Imath::Vec3<T>& T2,
										   T                     s,
										   Imath::Vec3<T>&       P) const
	{
		T h1 =  2.0*std::pow(s,3.0) - 3.0*std::pow(s,2.0) + 1.0; // calculate basis function 1
		T h2 = -2.0*std::pow(s,3.0) + 3.0*std::pow(s,2.0);       // calculate basis function 2
		T h3 =      std::pow(s,3.0) - 2.0*std::pow(s,2.0) + s;   // calculate basis function 3
		T h4 =      std::pow(s,3.0) -     std::pow(s,2.0);       // calculate basis function 4


		P = h1*P1 + // multiply and sum all functions
			h2*P2 + // together to build the interpolated
			h3*T1 + // point along the curve.
			h4*T2;
	}

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
