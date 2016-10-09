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
