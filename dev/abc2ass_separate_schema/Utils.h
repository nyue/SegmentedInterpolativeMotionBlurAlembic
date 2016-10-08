#pragma once
#include "Types.h"

bool build_even_motion_relative_time_samples(float           i_relative_shutter_open,
		float           i_relative_shutter_close,
		size_t          i_motion_sample_count,
		FloatContainer& o_earlier_sampling_time_vector,
		FloatContainer& o_later_sampling_time_vector);

template <typename T> void interpolate(const Imath::Vec3<T>& P1,
									   const Imath::Vec3<T>& T1,
									   const Imath::Vec3<T>& P2,
									   const Imath::Vec3<T>& T2,
									   T                     s,
									   Imath::Vec3<T>&       P)
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
