#include "AbstractGeomSchemaHandler.h"

AbstractGeomSchemaHandler::AbstractGeomSchemaHandler()
{

}

AbstractGeomSchemaHandler::~AbstractGeomSchemaHandler()
{

}

void AbstractGeomSchemaHandler::flatten_string_array(const StringContainer& i_string_array,
													 const std::string&     i_separator,
													 std::string&           o_flatten_string) const
{
	size_t num_strings = i_string_array.size();
	for (size_t index=0;index<num_strings;index++)
	{
		o_flatten_string.append(i_string_array[index]);
		if (index<(num_strings-1)) // We want to avoid a trailing separator!
			o_flatten_string.append(i_separator);
	}
}

bool AbstractGeomSchemaHandler::build_even_motion_relative_time_samples(float           i_relative_shutter_open,
																		float           i_relative_shutter_close,
																		size_t          i_motion_sample_count,
																		FloatContainer& o_earlier_sampling_time_vector,
																		FloatContainer& o_later_sampling_time_vector) const
{
	if (i_motion_sample_count<2)
		return false;
	float shutter_delta = (i_relative_shutter_close - i_relative_shutter_open)/(i_motion_sample_count-1);
	if (shutter_delta < FLT_EPSILON)
		return false;
	o_earlier_sampling_time_vector.clear();
	o_later_sampling_time_vector.clear();
	for (size_t sample_index=0;sample_index<i_motion_sample_count;sample_index++)
	{
		float time_sample  = i_relative_shutter_open + sample_index * shutter_delta;
		std::cout << boost::format("time_sample = %1%") % time_sample << std::endl;
		if (time_sample>=0.0f)
			o_later_sampling_time_vector.push_back(time_sample);
		else
			o_earlier_sampling_time_vector.push_back(time_sample);
	}
	return true;
}
