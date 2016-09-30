#include <ai.h>

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <cassert>

#include <boost/format.hpp>
#include <boost/multi_array.hpp>

#include <OpenEXR/ImathVec.h>

typedef boost::multi_array<Imath::V3f, 2> V3fSamplingArray2D;

//points
//{
// name test
// points 2 3 POINT
// # 2 points
// # 3 motion blur samples per point
// # First sample
// -10 0 0
// 10 0 0
// # Second sample
// -10 5 0
// 10 15 0
// # Third sample
// -15 5 0
// 15 10 0
// radius 1
// deform_time_samples 2 1 FLOAT
//-0.25 0.25
//}

int main(int argc, char **argv)
{
	V3fSamplingArray2D arnold_points_data_array;

	V3fSamplingArray2D::index i_motion_sample_count = 3;
	V3fSamplingArray2D::index i_current_interim_points_count = 2;
	arnold_points_data_array.resize(boost::extents[i_motion_sample_count][i_current_interim_points_count]);

	std::string i_arnold_filename("points.ass");
	std::string name("mypoints");

	// Sample 0
	arnold_points_data_array[0][0] = Imath::V3f(-10, 0, 0);
	arnold_points_data_array[0][1] = Imath::V3f( 10, 0, 0);

	// Sample 1
	arnold_points_data_array[1][0] = Imath::V3f(-10,  5, 0);
	arnold_points_data_array[1][1] = Imath::V3f( 10, 15, 0);

	// Sample 2
	arnold_points_data_array[2][0] = Imath::V3f(-15,  5, 0);
	arnold_points_data_array[2][1] = Imath::V3f( 15, 10, 0);

	// start an Arnold session
	AiBegin();

	// name
	AtNode *points = AiNode("points");
	AiNodeSetStr(points, "name", name.c_str());

	// points
	AiNodeSetArray(points, "points", AiArrayConvert(i_current_interim_points_count,i_motion_sample_count,AI_TYPE_POINT,arnold_points_data_array.data()));

	// write out an .ass file
	AiASSWrite(i_arnold_filename.c_str(), AI_NODE_ALL, false, false);

	// at this point we can shut down Arnold
	AiEnd();


	return 0;
}
