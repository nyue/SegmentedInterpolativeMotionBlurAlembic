#include <cfloat>
#include <boost/format.hpp>
#include "Utils.h"

bool build_even_motion_relative_time_samples(float                 i_relative_shutter_open,
											 float                 i_relative_shutter_close,
											 Alembic::Abc::uint8_t i_motion_sample_count,
											 FloatContainer&       o_earlier_sampling_time_vector,
											 FloatContainer&       o_later_sampling_time_vector)
{
	if (i_motion_sample_count<2)
		return false;
	float shutter_delta = (i_relative_shutter_close - i_relative_shutter_open)/(i_motion_sample_count-1);
	if (shutter_delta < FLT_EPSILON)
		return false;
	o_earlier_sampling_time_vector.clear();
	o_later_sampling_time_vector.clear();
	for (Alembic::Abc::uint8_t sample_index=0;sample_index<i_motion_sample_count;sample_index++)
	{
		float time_sample  = i_relative_shutter_open + sample_index * shutter_delta;
		// std::cout << boost::format("time_sample = %1%") % time_sample << std::endl;
		if (time_sample>=0.0f)
			o_later_sampling_time_vector.push_back(time_sample);
		else
			o_earlier_sampling_time_vector.push_back(time_sample);
	}
	return true;

}

bool build_single_even_motion_relative_time_samples(float                 i_relative_shutter_open,
													float                 i_relative_shutter_close,
													Alembic::Abc::uint8_t i_motion_sample_count,
													FloatContainer&       o_sampling_time_vector)
{
	if (i_motion_sample_count<2)
		return false;
	float shutter_delta = (i_relative_shutter_close - i_relative_shutter_open)/(i_motion_sample_count-1);
	if (shutter_delta < FLT_EPSILON)
		return false;
	o_sampling_time_vector.clear();
	for (Alembic::Abc::uint8_t sample_index=0;sample_index<i_motion_sample_count;sample_index++)
	{
		float time_sample  = i_relative_shutter_open + sample_index * shutter_delta;
		// std::cout << boost::format("time_sample = %1%") % time_sample << std::endl;
		o_sampling_time_vector.push_back(time_sample);
	}
	return true;

}

void build_interim_points(const Alembic::AbcGeom::IPointsSchema::Sample* i_sample,
						  AlembicPointsDataIndexedMap&                   o_interim_points)
{
	Alembic::AbcGeom::P3fArraySamplePtr positions = i_sample->getPositions();
	Alembic::AbcGeom::V3fArraySamplePtr velocities = i_sample->getVelocities();
	Alembic::AbcGeom::UInt64ArraySamplePtr ids = i_sample->getIds();
	size_t num_positions = positions->size();
	size_t num_velocities = velocities->size();
	size_t num_ids = ids->size();
	assert ( num_positions == num_ids );
	assert ( num_velocities == num_ids );
	std::pair<AlembicPointsDataIndexedMap::iterator,bool> ret;
	for (size_t positions_index = 0;positions_index < num_positions; positions_index++)
	{
		ret = o_interim_points.insert(AlembicPointsDataIndexedMap::value_type(
				ids->get()[positions_index],
				AlembicPointsData(Imath::V3f(positions->get()[positions_index].x,positions->get()[positions_index].y,positions->get()[positions_index].z),
						Imath::V3f(velocities->get()[positions_index].x,velocities->get()[positions_index].y,velocities->get()[positions_index].z))));
		if (!ret.second)
		{
			std::cerr << "Failed to insert into map" << std::endl;
			return;
		}
	}
	// std::cout << boost::format("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX o_interim_points = %1%") % o_interim_points.size() << std::endl;
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
