#include <ai.h>

#include <Alembic/Util/All.h>
#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#ifdef SIMBA_ENABLE_ALEMBIC_HDF5
#include <Alembic/AbcCoreHDF5/All.h>
#endif // SIMBA_ENABLE_ALEMBIC_HDF5
#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCollection/All.h>
#include <Alembic/AbcMaterial/All.h>

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <cassert>

#include <boost/format.hpp>
#include <boost/multi_array.hpp>

#include <OpenEXR/ImathVec.h>

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

typedef boost::multi_array<Imath::V3f, 2> V3fSamplingArray2D;
typedef std::vector<AtUInt32> AtUInt32Container;
typedef std::vector<uint64_t> UInt64Container;
typedef std::vector<std::string> StringContainer;
typedef std::vector<float> FloatContainer;
struct ArnoldMeshData
{
	V3fSamplingArray2D _vlist_data_array;
	// Topological stable being assumed
	AtUInt32Container _nsides_data;
	AtUInt32Container _vidxs_data;
};
struct ArnoldPointsData
{
	V3fSamplingArray2D _points_data_array;
	// Topological stable being assumed
	UInt64Container _ids_data;
	FloatContainer _radius_data;
};
struct AlembicPointsData
{
	AlembicPointsData(const Imath::V3f& i_position, const Imath::V3f& i_velocity)
	: _position(i_position)
	, _velocity(i_velocity)
	{}
	Imath::V3f _position;
	Imath::V3f _velocity;
};
typedef std::map<uint64_t,AlembicPointsData> AlembicPointsDataIndexedMap;

void make_arnold_points(const std::string& 		name,
						const ArnoldPointsData& i_arnold_points_data,
						float 					i_shutter_open,
						float 					i_shutter_close)
{
	AtByte nkeys = i_arnold_points_data._points_data_array.size();
	if (nkeys<1)
		return;
	// Assumes topological stability
	AtUInt32 nelements = i_arnold_points_data._points_data_array[0].size();
	// name
	AtNode *points = AiNode("points");
	AiNodeSetStr(points, "name", name.c_str());

	std::cout << boost::format("nelements=%1% nkeys=%2%") % nelements % int(nkeys)<< std::endl;

	// points
	AiNodeSetArray(points, "points", AiArrayConvert(nelements,nkeys,AI_TYPE_POINT,i_arnold_points_data._points_data_array.data()));

	// radius
	AiNodeSetArray(points, "radius", AiArrayConvert(i_arnold_points_data._radius_data.size(),1,AI_TYPE_FLOAT,i_arnold_points_data._radius_data.data()));

	// deform_time_samples
	float deform_time_samples[2] = {i_shutter_open,i_shutter_close};
	AiNodeSetArray(points, "deform_time_samples", AiArrayConvert(2,1,AI_TYPE_FLOAT,deform_time_samples));
}

void make_arnold_polymesh(const std::string& name,
						  const ArnoldMeshData& i_arnold_mesh_data,
						  float i_shutter_open,
						  float i_shutter_close)
{
	AtByte nkeys = i_arnold_mesh_data._vlist_data_array.size();
	if (nkeys<1)
		return;
	// Assumes topological stability
	AtUInt32 nelements = i_arnold_mesh_data._vlist_data_array[0].size();
	// name
	AtNode *polymesh = AiNode("polymesh");
	AiNodeSetStr(polymesh, "name", name.c_str());

	std::cout << boost::format("nelements=%1% nkeys=%2%") % nelements % int(nkeys)<< std::endl;

	// vlist
	AiNodeSetArray(polymesh, "vlist", AiArrayConvert(nelements,nkeys,AI_TYPE_POINT,i_arnold_mesh_data._vlist_data_array.data()));

	// nsides
	AiNodeSetArray(polymesh, "nsides", AiArrayConvert(i_arnold_mesh_data._nsides_data.size(),1,AI_TYPE_UINT,i_arnold_mesh_data._nsides_data.data()));

	// vidxs
	AiNodeSetArray(polymesh, "vidxs", AiArrayConvert(i_arnold_mesh_data._vidxs_data.size(),1,AI_TYPE_UINT,i_arnold_mesh_data._vidxs_data.data()));

	// deform_time_samples
	float deform_time_samples[2] = {i_shutter_open,i_shutter_close};
	AiNodeSetArray(polymesh, "deform_time_samples", AiArrayConvert(2,1,AI_TYPE_FLOAT,deform_time_samples));
}

void write_arnold_points_data_to_csv_file(const ArnoldPointsData&   i_arnold_points_data,
										  V3fSamplingArray2D::index i_sample_index,
										  const std::string&        i_csv_filename)
{
	std::ofstream csv_file;
	csv_file.open (i_csv_filename.c_str());

    size_t per_sample_point_count = i_arnold_points_data._points_data_array[i_sample_index].size();
    size_t radius_count = i_arnold_points_data._radius_data.size();

    assert ( per_sample_point_count == radius_count );

    csv_file << "x,y,z,width" << std::endl;

    for (size_t index=0;index<per_sample_point_count;index++)
    {
    	float x = i_arnold_points_data._points_data_array[i_sample_index][index].x;
    	float y = i_arnold_points_data._points_data_array[i_sample_index][index].y;
    	float z = i_arnold_points_data._points_data_array[i_sample_index][index].z;
    	float w = i_arnold_points_data._radius_data[index] * 2.0f;
        csv_file << boost::format("%1%, %2%, %3%, %4%") % x % y % z % w << std::endl;
    }

    csv_file.close();
}

void write_arnold_points_data_to_csv_sequence(const ArnoldPointsData& i_arnold_points_data,
												  const std::string&    i_base_filename)
{
	std::cout << "NICHOLAS write_arnold_points_data_to_csv_sequence() XXXXXXXXXXXXXXXXX" << std::endl;
	V3fSamplingArray2D::index num_samples = i_arnold_points_data._points_data_array.size();
	V3fSamplingArray2D::index num_elements_per_sample = i_arnold_points_data._points_data_array.shape()[1];
	size_t num_radii = i_arnold_points_data._radius_data.size();
	size_t num_ids = i_arnold_points_data._ids_data.size();
	if ( num_radii != num_elements_per_sample )
	{
		std::cerr << boost::format("num_radii = %1% != num_elements_per_sample = %2%") % num_radii % num_elements_per_sample << std::endl;
		return;
	}
	if ( num_ids != num_elements_per_sample )
	{
		std::cerr << boost::format("num_ids = %1% != num_elements_per_sample = %2%") % num_ids % num_elements_per_sample << std::endl;
		return;
	}
	// std::cout << boost::format("num_elements_per_sample = %1%") % num_elements_per_sample << std::endl;
	for (V3fSamplingArray2D::index sample_index=0;sample_index<num_samples;++sample_index)
	{
		std::string numbered_output_filename = (boost::format(i_base_filename) % (sample_index+1)).str();
		// std::cout << boost::format("numbered_output_filename = '%1%'") % numbered_output_filename << std::endl;
		write_arnold_points_data_to_csv_file(i_arnold_points_data,sample_index,numbered_output_filename);
	}
	// std::cout << boost::format("_vlist_data_array num_elements = %1%") % i_arnold_mesh_data._vlist_data_array.size() << std::endl;
}

void write_arnold_mesh_data_to_wavefront_file(const ArnoldMeshData&     i_arnold_mesh_data,
											  V3fSamplingArray2D::index i_sample_index,
											  const std::string&        i_wavefront_filename)
{

	std::ofstream wavefront_file;
	wavefront_file.open (i_wavefront_filename.c_str());

	wavefront_file << "# File exported by Nicholas Yue\n";


    size_t P_size = i_arnold_mesh_data._vlist_data_array[i_sample_index].size();
    size_t indices_size = i_arnold_mesh_data._vidxs_data.size();
    size_t counts_size = i_arnold_mesh_data._nsides_data.size();

    wavefront_file << boost::format("# %1% points") % P_size << std::endl;
    wavefront_file << boost::format("# %1% faces") % counts_size << std::endl;

    wavefront_file << "g" << std::endl;
    for (size_t index=0;index<P_size;index++)
    {
    	float x = i_arnold_mesh_data._vlist_data_array[i_sample_index][index].x;
    	float y = i_arnold_mesh_data._vlist_data_array[i_sample_index][index].y;
    	float z = i_arnold_mesh_data._vlist_data_array[i_sample_index][index].z;
        wavefront_file << boost::format("v %1% %2% %3%") % x % y % z << std::endl;
    }

    wavefront_file << "g" << std::endl;
    size_t indices_index = 0;
    for (size_t index=0;index<counts_size;index++)
    {
        wavefront_file << "f";
    	size_t face_vert_count = i_arnold_mesh_data._nsides_data[index];
    	for (size_t face_vert_index=0;face_vert_index < face_vert_count; ++face_vert_index)
    	{
    		size_t face_vert_index_value = i_arnold_mesh_data._vidxs_data[indices_index] + 1; // OBJ array is not zero based!
    		wavefront_file << boost::format(" %1%") % face_vert_index_value;
    		indices_index++;
    	}
		wavefront_file << std::endl;
    }
    wavefront_file.close();
}

void write_arnold_mesh_data_to_wavefront_sequence(const ArnoldMeshData& i_arnold_mesh_data,
												  const std::string&    i_base_filename)
{

	V3fSamplingArray2D::index num_samples = i_arnold_mesh_data._vlist_data_array.size();
	V3fSamplingArray2D::index num_elements_per_sample = i_arnold_mesh_data._vlist_data_array.shape()[1];
	std::cout << boost::format("num_elements_per_sample = %1%") % num_elements_per_sample << std::endl;
	for (V3fSamplingArray2D::index sample_index=0;sample_index<num_samples;++sample_index)
	{
		std::string numbered_output_filename = (boost::format(i_base_filename) % (sample_index+1)).str();
		std::cout << boost::format("numbered_output_filename = '%1%'") % numbered_output_filename << std::endl;
		write_arnold_mesh_data_to_wavefront_file(i_arnold_mesh_data,sample_index,numbered_output_filename);
	}
	std::cout << boost::format("_vlist_data_array num_elements = %1%") % i_arnold_mesh_data._vlist_data_array.size() << std::endl;
}

void write_arnold_points_data_to_file(const ArnoldPointsData& i_arnold_points_data,
									  const std::string&      i_arnold_filename,
									  float 				  i_shutter_open,
									  float 				  i_shutter_close)
{
	// start an Arnold session
	AiBegin();
//	AiMsgSetLogFileName("pm.log");

	make_arnold_points("test",i_arnold_points_data,i_shutter_open,i_shutter_close);
#ifdef FULL_ARNOLD_SCENE
	// create a sphere geometric primitive
	AtNode *sph = AiNode("sphere");
	AiNodeSetStr(sph, "name", "mysphere");
	AiNodeSetFlt(sph, "radius", 5.0f);

	// create a lambert shader
	AtNode *shader = AiNode("lambert");
	AiNodeSetStr(shader, "name", "mylambert");
	AiNodeSetRGB(shader, "Kd_color", 1.0f, 0.0f, 0.0f);

	// assign the sphere's shader
	AiNodeSetPtr(sph, "shader", shader);

	// create a perspective camera
	AtNode *camera = AiNode("persp_camera");
	AiNodeSetStr(camera, "name", "mycamera");
	// position the camera (alternatively you can set 'matrix')
	AiNodeSetPnt(camera, "position", 0.f, 0.f, 60.f);

	// create a point light source
	AtNode *light = AiNode("point_light");
	AiNodeSetStr(light, "name", "mylight");
	// position the light (alternatively use 'matrix')
	AiNodeSetPnt(light, "position", 0.f, 10.f, 10.f);

	// get the global options node and set some options
	AtNode *options = AiUniverseGetOptions();
	AiNodeSetInt(options, "AA_samples", 1);
	AiNodeSetInt(options, "xres", 320);
	AiNodeSetInt(options, "yres", 240);
	// set the active camera (optional, since there is only one camera)
	AiNodeSetPtr(options, "camera", camera);

	// create an output driver node
	AtNode *driver = AiNode("driver_tiff");
	AiNodeSetStr(driver, "name", "mydriver");
	AiNodeSetStr(driver, "filename", "pm.tiff");

	// create a gaussian filter node
	AtNode *filter = AiNode("gaussian_filter");
	AiNodeSetStr(filter, "name", "myfilter");

	// assign the driver and filter to the main (beauty) AOV, which is called "RGB"
	AtArray *outputs_array = AiArrayAllocate(1, 1, AI_TYPE_STRING);
	AiArraySetStr(outputs_array, 0, "RGBA RGBA myfilter mydriver");
	AiNodeSetArray(options, "outputs", outputs_array);

	// finally, render the image
	// AiRender(AI_RENDER_MODE_CAMERA);
#endif // FULL_ARNOLD_SCENE

	// ... or you can write out an .ass file instead
	AiASSWrite(i_arnold_filename.c_str(), AI_NODE_ALL, false, false);

	// at this point we can shut down Arnold
	AiEnd();

}

void write_arnold_mesh_data_to_file(const ArnoldMeshData& i_arnold_mesh_data,
									const std::string&    i_arnold_filename,
									float 				  i_shutter_open,
									float 				  i_shutter_close)
{
	// start an Arnold session
	AiBegin();
	AiMsgSetLogFileName("pm.log");

	make_arnold_polymesh("test",i_arnold_mesh_data,i_shutter_open,i_shutter_close);

	// create a sphere geometric primitive
	AtNode *sph = AiNode("sphere");
	AiNodeSetStr(sph, "name", "mysphere");
	AiNodeSetFlt(sph, "radius", 5.0f);

	// create a lambert shader
	AtNode *shader = AiNode("lambert");
	AiNodeSetStr(shader, "name", "mylambert");
	AiNodeSetRGB(shader, "Kd_color", 1.0f, 0.0f, 0.0f);

	// assign the sphere's shader
	AiNodeSetPtr(sph, "shader", shader);

	// create a perspective camera
	AtNode *camera = AiNode("persp_camera");
	AiNodeSetStr(camera, "name", "mycamera");
	// position the camera (alternatively you can set 'matrix')
	AiNodeSetPnt(camera, "position", 0.f, 0.f, 60.f);

	// create a point light source
	AtNode *light = AiNode("point_light");
	AiNodeSetStr(light, "name", "mylight");
	// position the light (alternatively use 'matrix')
	AiNodeSetPnt(light, "position", 0.f, 10.f, 10.f);

	// get the global options node and set some options
	AtNode *options = AiUniverseGetOptions();
	AiNodeSetInt(options, "AA_samples", 1);
	AiNodeSetInt(options, "xres", 320);
	AiNodeSetInt(options, "yres", 240);
	// set the active camera (optional, since there is only one camera)
	AiNodeSetPtr(options, "camera", camera);

	// create an output driver node
	AtNode *driver = AiNode("driver_tiff");
	AiNodeSetStr(driver, "name", "mydriver");
	AiNodeSetStr(driver, "filename", "pm.tiff");

	// create a gaussian filter node
	AtNode *filter = AiNode("gaussian_filter");
	AiNodeSetStr(filter, "name", "myfilter");

	// assign the driver and filter to the main (beauty) AOV, which is called "RGB"
	AtArray *outputs_array = AiArrayAllocate(1, 1, AI_TYPE_STRING);
	AiArraySetStr(outputs_array, 0, "RGBA RGBA myfilter mydriver");
	AiNodeSetArray(options, "outputs", outputs_array);

	// finally, render the image
	// AiRender(AI_RENDER_MODE_CAMERA);

	// ... or you can write out an .ass file instead
	AiASSWrite(i_arnold_filename.c_str(), AI_NODE_ALL, false, false);

	// at this point we can shut down Arnold
	AiEnd();

}

bool build_even_motion_relative_time_samples(float i_relative_shutter_open,
											 float i_relative_shutter_close,
											 AtByte i_motion_sample_count,
											 FloatContainer& o_earlier_sampling_time_vector,
											 FloatContainer& o_later_sampling_time_vector)
{
	if (i_motion_sample_count<2)
		return false;
	float shutter_delta = (i_relative_shutter_close - i_relative_shutter_open)/(i_motion_sample_count-1);
	if (shutter_delta < FLT_EPSILON)
		return false;
	o_earlier_sampling_time_vector.clear();
	o_later_sampling_time_vector.clear();
	for (AtByte sample_index=0;sample_index<i_motion_sample_count;sample_index++)
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

/*!
 * \param i_relative_shutter_open This can be negative (relative to the current frame)
 * \param i_relative_shutter_close This can be negative (relative to the current frame)
 */
void build_interim_points_for_arnold_ass(const Alembic::AbcGeom::IPointsSchema::Sample* i_sample,
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

// Arnold point data layout in the case of multiple points with multiple motion blur segments
//
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
void build_points_for_arnold_ass_from_interim_points(const AlembicPointsDataIndexedMap* i_previous_interim_points,
													 const AlembicPointsDataIndexedMap* i_current_interim_points,
													 const AlembicPointsDataIndexedMap* i_next_interim_points,
													 float								i_relative_shutter_open,
													 float								i_relative_shutter_close,
													 AtByte								i_motion_sample_count,
													 ArnoldPointsData&					o_arnold_points)
{

	// Use the current interim point's id as lookup for the previous and next information
//	V3fSamplingArray2D::index num_test_samples = i_motion_sample_count;
//	std::cout << boost::format("YYYYYYYYYYYYYYYYYYYYYY num_test_samples = %1%") % num_test_samples << std::endl;
//	num_test_samples = 1;

	FloatContainer earlier_sampling_time_vector;
	FloatContainer later_sampling_time_vector;


	if (build_even_motion_relative_time_samples(i_relative_shutter_open,
												i_relative_shutter_close,
												i_motion_sample_count,
												earlier_sampling_time_vector,
												later_sampling_time_vector))
	{
		{
			size_t num_earlier_samples = earlier_sampling_time_vector.size();
			for (size_t index=0;index<num_earlier_samples;++index)
			{
				std::cout << boost::format("earlier_sampling_time_vector[%1%] = %2%") % index % earlier_sampling_time_vector[index] << std::endl;
			}
		}
		{
			size_t num_later_samples = later_sampling_time_vector.size();
			for (size_t index=0;index<num_later_samples;++index)
			{
				std::cout << boost::format("later_sampling_time_vector[%1%] = %2%") % index % later_sampling_time_vector[index] << std::endl;
			}
		}
	}
	o_arnold_points._points_data_array.resize(boost::extents[i_motion_sample_count][i_current_interim_points->size()]);
	V3fSamplingArray2D::index point_index = 0;
	for(AlembicPointsDataIndexedMap::const_iterator iter = i_current_interim_points->begin(); iter != i_current_interim_points->end(); iter++,point_index++)
	{
		uint64_t search_id = iter->first;
		o_arnold_points._ids_data.push_back(search_id);
		o_arnold_points._radius_data.push_back(0.01f);
		bool previous_point_exists = false;
		bool next_point_exists = false;
		// Generate samples for this particular point
		typedef std::vector<Imath::V3f> V3fContainer;

		V3fContainer interpolated_P(i_motion_sample_count);
		AlembicPointsDataIndexedMap::const_iterator find_previous_point_result;
		AlembicPointsDataIndexedMap::const_iterator find_next_point_result;
		if (i_previous_interim_points)
		{
			find_previous_point_result = i_previous_interim_points->find(search_id);
	        if (find_previous_point_result != i_previous_interim_points->end())
	        {
	        	previous_point_exists = true;
	        }
		}

		if (i_next_interim_points)
		{
			find_next_point_result = i_next_interim_points->find(search_id);
	        if (find_next_point_result != i_next_interim_points->end())
	        {
	        	next_point_exists = true;
	        }
		}

		// The are 3 possibilities - start, in-between and end
		if (!previous_point_exists && next_point_exists)
		{
			// START frame
			std::cout << "START frame : points processing" << std::endl;
		}
		else if (previous_point_exists && !next_point_exists)
		{
			// END frame
			std::cout << "END frame : points processing" << std::endl;
		}
		else if (previous_point_exists && next_point_exists)
		{
			// INBETWEEN frame
			std::cout << "INBETWEEN frame : points processing" << std::endl;
			size_t interpolated_P_index = 0;
			size_t num_earlier_sampling_time = earlier_sampling_time_vector.size();
			size_t num_later_sampling_time = later_sampling_time_vector.size();

			Imath::V3f P1;
			Imath::V3f T1;
			Imath::V3f P2;
			Imath::V3f T2;

			// Previous
			P1 = find_previous_point_result->second._position;
			T1 = find_previous_point_result->second._velocity;
			P2 = iter->second._position;
			T2 = iter->second._velocity;
			for (size_t earlier_sample_index=0;earlier_sample_index<num_earlier_sampling_time;++earlier_sample_index)
			{
				float s = 1+earlier_sampling_time_vector[earlier_sample_index];
				interpolate<float>(P1,T1,P2,T2,s,interpolated_P[interpolated_P_index]);
				interpolated_P_index++;
			}

			// Next
			P1 = iter->second._position;
			T1 = iter->second._velocity;
			P2 = find_next_point_result->second._position;
			T2 = find_next_point_result->second._velocity;
			for (size_t next_sample_index=0;next_sample_index<num_later_sampling_time;++next_sample_index)
			{
				float s = later_sampling_time_vector[next_sample_index];
				interpolate<float>(P1,T1,P2,T2,s,interpolated_P[interpolated_P_index]);
				interpolated_P_index++;
			}

			assert ( interpolated_P_index == i_motion_sample_count );
		}
		for (V3fSamplingArray2D::index motion_index = 0;motion_index<i_motion_sample_count;++motion_index)
		{
			o_arnold_points._points_data_array[motion_index][point_index] = interpolated_P[motion_index];
		}
	}
}

///*!
// * \param i_relative_shutter_open This can be negative (relative to the current frame)
// * \param i_relative_shutter_close This can be negative (relative to the current frame)
// */
//void build_points_for_arnold_ass(const Alembic::AbcGeom::IPointsSchema::Sample* i_previous_sample,
//								 const Alembic::AbcGeom::IPointsSchema::Sample* i_current_sample,
//								 const Alembic::AbcGeom::IPointsSchema::Sample* i_next_sample,
//								 float											i_relative_shutter_open,
//								 float											i_relative_shutter_close,
//								 AtByte											i_motion_sample_count,
//								 ArnoldPointsData&                              o_arnold_points)
//{
//
//}
/*!
 * \param i_relative_shutter_open This can be negative (relative to the current frame)
 * \param i_relative_shutter_close This can be negative (relative to the current frame)
 */
void build_polymesh_for_arnold_ass(const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_previous_sample,
								   const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_current_sample,
								   const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_next_sample,
								   float											i_relative_shutter_open,
								   float											i_relative_shutter_close,
								   AtByte											i_motion_sample_count,
								   ArnoldMeshData&                                  o_arnold_mesh)
{
	std::cout << "INTERPOLATE 0000" << std::endl;
	if (!i_current_sample)
		return;

	std::cout << "INTERPOLATE 0100" << std::endl;
	// Assumes topologically stable
	Alembic::AbcGeom::Int32ArraySamplePtr indices = i_current_sample->getFaceIndices();
	Alembic::AbcGeom::Int32ArraySamplePtr counts = i_current_sample->getFaceCounts();

	// Current
	Alembic::AbcGeom::P3fArraySamplePtr current_P = i_current_sample->getPositions();
	Alembic::AbcGeom::V3fArraySamplePtr current_v = i_current_sample->getVelocities();
	size_t current_num_P = current_P->size();
	size_t current_num_v = current_v->size();

	// Topologically constant
	size_t num_nsides = counts->size();
	size_t num_indices = indices->size();
	o_arnold_mesh._nsides_data.resize(num_nsides);
	o_arnold_mesh._vidxs_data.resize(num_indices);
	for (size_t index=0;index<num_nsides;index++)
	{
		o_arnold_mesh._nsides_data[index] = counts->get()[index];
	}
	for (size_t index=0;index<num_indices;index++)
	{
		o_arnold_mesh._vidxs_data[index] = indices->get()[index];
	}

	if ( i_motion_sample_count == 1)
	{
		// Special case, return single time step from current sample
		std::cout << boost::format("num_nsides=%1% num_indices=%2% num_P=%3%") % num_nsides % num_indices % current_num_P << std::endl;
		o_arnold_mesh._vlist_data_array.resize(boost::extents[i_motion_sample_count][current_num_P]);
		for (size_t index=0;index<current_num_P;index++)
		{
			o_arnold_mesh._vlist_data_array[0][index].x = current_P->get()[index].x;
			o_arnold_mesh._vlist_data_array[0][index].y = current_P->get()[index].y;
			o_arnold_mesh._vlist_data_array[0][index].z = current_P->get()[index].z;
		}

		return;
	}

	if (i_previous_sample && i_next_sample)
	{
		std::cout << "INTERPOLATE INBETWEEN" << std::endl;

		Alembic::AbcGeom::P3fArraySamplePtr previous_P = i_previous_sample->getPositions();
		Alembic::AbcGeom::V3fArraySamplePtr previous_v = i_previous_sample->getVelocities();
		size_t previous_num_P = previous_P->size();
		size_t previous_num_v = previous_v->size();

		Alembic::AbcGeom::P3fArraySamplePtr next_P = i_next_sample->getPositions();
		Alembic::AbcGeom::V3fArraySamplePtr next_v = i_next_sample->getVelocities();
		size_t next_num_P = next_P->size();
		size_t next_num_v = next_v->size();

		if (	(previous_num_P==current_num_P)
				&&
				(next_num_P==current_num_P)
				&&
				(previous_num_v==current_num_v)
				&&
				(next_num_v==current_num_v)
				)
		{

			FloatContainer earlier_sampling_time_vector;
			FloatContainer later_sampling_time_vector;
			build_even_motion_relative_time_samples(i_relative_shutter_open,
													i_relative_shutter_close,
													i_motion_sample_count,
													earlier_sampling_time_vector,
													later_sampling_time_vector);

			o_arnold_mesh._vlist_data_array.resize(boost::extents[i_motion_sample_count][current_num_P]);
			assert(i_motion_sample_count==(earlier_sampling_time_vector.size()+later_sampling_time_vector.size()));
			FloatContainer::const_iterator tsIter;
			FloatContainer::const_iterator tsEIter;
			size_t sampling_index = 0; // shared over both earlier_sampling_time_vector and later_sampling_time_vector

			// Start with earlier time sample first so it is monotonically increasing

			// EARLIER
			tsIter = earlier_sampling_time_vector.begin();
			tsEIter = earlier_sampling_time_vector.end();
			for (;tsIter!=tsEIter;++tsIter)
			{

				for (size_t index=0;index<current_num_P;index++)
				{
					Imath::Vec3<float> P1(previous_P->get()[index].x,previous_P->get()[index].y,previous_P->get()[index].z);
					Imath::Vec3<float> T1(previous_v->get()[index].x,previous_v->get()[index].y,previous_v->get()[index].z);
					Imath::Vec3<float> P2(current_P->get()[index].x,current_P->get()[index].y,current_P->get()[index].z);
					Imath::Vec3<float> T2(current_v->get()[index].x,current_v->get()[index].y,current_v->get()[index].z);
					Imath::Vec3<float> P;
					float s = 1+(*tsIter);
					std::cout << boost::format("EARLIER s = %1%") % s << std::endl;
					interpolate<float>(P1,T1,P2,T2,s,P);
					std::cout << boost::format("Earlier [sampling_index = %1%] P1 = %2%, T1 = %3%, P2 = %4%, T2 = %5% [s = %6%] {P =%7%}") % sampling_index % P1 % T1 % P2 % T2 % s % P << std::endl;


					o_arnold_mesh._vlist_data_array[sampling_index][index].x = P.x;
					o_arnold_mesh._vlist_data_array[sampling_index][index].y = P.y;
					o_arnold_mesh._vlist_data_array[sampling_index][index].z = P.z;
				}

				sampling_index++;

			}

			std::cout << "Earlier/Later =================================" << std::endl;

			// LATER
			tsIter = later_sampling_time_vector.begin();
			tsEIter = later_sampling_time_vector.end();
			for (;tsIter!=tsEIter;++tsIter)
			{
				for (size_t index=0;index<current_num_P;index++)
				{

					Imath::Vec3<float> P1(current_P->get()[index].x,current_P->get()[index].y,current_P->get()[index].z);
					Imath::Vec3<float> T1(current_v->get()[index].x,current_v->get()[index].y,current_v->get()[index].z);
					Imath::Vec3<float> P2(next_P->get()[index].x,next_P->get()[index].y,next_P->get()[index].z);
					Imath::Vec3<float> T2(next_v->get()[index].x,next_v->get()[index].y,next_v->get()[index].z);
					Imath::Vec3<float> P;
					float s = *tsIter;
					// std::cout << boost::format("s = %1%") % s << std::endl;
					interpolate<float>(P1,T1,P2,T2,s,P);
					std::cout << boost::format("Later [sampling_index = %1%] P1 = %2%, T1 = %3%, P2 = %4%, T2 = %5% [s = %6%] {P =%7%}") % sampling_index % P1 % T1 % P2 % T2 % s % P << std::endl;


					o_arnold_mesh._vlist_data_array[sampling_index][index].x = P.x;
					o_arnold_mesh._vlist_data_array[sampling_index][index].y = P.y;
					o_arnold_mesh._vlist_data_array[sampling_index][index].z = P.z;
				}

				sampling_index++;
			}
		}
	}
	else if ((i_previous_sample == 0) && i_next_sample)
	{
		std::cout << "INTERPOLATE FIRST FRAME" << std::endl;
	}
	else if (i_previous_sample && (i_next_sample == 0))
	{
		std::cout << "INTERPOLATE LAST FRAME" << std::endl;
	}

}

void print_sample_data(const std::string& i_message,const Alembic::AbcGeom::IPolyMeshSchema::Sample& i_sample)
{
	Alembic::AbcGeom::P3fArraySamplePtr sample_P = i_sample.getPositions();
	Alembic::AbcGeom::V3fArraySamplePtr sample_v = i_sample.getVelocities();
	size_t sample_num_P = sample_P->size();
	size_t sample_num_v = sample_v->size();
	assert(sample_num_P==sample_num_v);
	for (size_t index=0;index<sample_num_P;index++)
	{
		std::cout << boost::format("[%1%] P = %2%, v = %3%") % i_message % sample_P->get()[index] % sample_v->get()[index] << std::endl;
	}

}

void export_points_as_arnold_ass(Alembic::AbcGeom::IPoints& points,
								 Alembic::Abc::index_t      i_start_frame_number,
								 Alembic::Abc::index_t      i_requested_frame_number,
								 const std::string&         i_arnold_filename,
								 AtByte						i_motion_samples,
								 float						i_relative_shutter_open,
								 float						i_relative_shutter_close)
{

    size_t num_samples = points.getSchema().getNumSamples();
    size_t last_sample_index = num_samples - 1;
    Alembic::Abc::int64_t requested_index = i_requested_frame_number - i_start_frame_number;
    std::cout << boost::format("num_samples = %1% i_requested_frame_number = %2% requested_index = %3% last_sample_index = %4%") % num_samples % i_requested_frame_number % requested_index % last_sample_index << std::endl;
    if (requested_index < 0 || requested_index > (num_samples-1))
    	return;

	Alembic::Abc::ISampleSelector current_sample_selector(requested_index);
    Alembic::AbcGeom::IPointsSchema::Sample current_sample;
    points.getSchema().get( current_sample, current_sample_selector );
    AlembicPointsDataIndexedMap current_interim_points;

    build_interim_points_for_arnold_ass(&current_sample,current_interim_points);

    if (requested_index == 0)
    {
    	// First frame
    	Alembic::Abc::ISampleSelector next_sample_selector(requested_index+1);

        Alembic::AbcGeom::IPointsSchema::Sample next_sample;
    	points.getSchema().get( next_sample, next_sample_selector );

        AlembicPointsDataIndexedMap next_interim_points;
        build_interim_points_for_arnold_ass(&next_sample,next_interim_points);

    	ArnoldPointsData arnold_points_data;
    	build_points_for_arnold_ass_from_interim_points(0,
    													&current_interim_points,
														&next_interim_points,
														i_relative_shutter_open,
														i_relative_shutter_close,
														i_motion_samples,
														arnold_points_data);
    	write_arnold_points_data_to_file(arnold_points_data,
    									 i_arnold_filename,
										 i_relative_shutter_open,
										 i_relative_shutter_close);

    }
    else if (requested_index == last_sample_index)
    {
    	// Last frame
    	Alembic::Abc::ISampleSelector previous_sample_selector(requested_index-1);

        Alembic::AbcGeom::IPointsSchema::Sample previous_sample;
    	points.getSchema().get( previous_sample, previous_sample_selector );

        AlembicPointsDataIndexedMap previous_interim_points;
        build_interim_points_for_arnold_ass(&previous_sample,previous_interim_points);

    	ArnoldPointsData arnold_points_data;
    	build_points_for_arnold_ass_from_interim_points(&previous_interim_points,
    													&current_interim_points,
														0,
														i_relative_shutter_open,
														i_relative_shutter_close,
														i_motion_samples,
														arnold_points_data);
    	write_arnold_points_data_to_file(arnold_points_data,
    									 i_arnold_filename,
										 i_relative_shutter_open,
										 i_relative_shutter_close);

    }
    else
    {
    	std::cout << "NICHOLAS XXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;
    	Alembic::Abc::ISampleSelector previous_sample_selector(requested_index-1);

        Alembic::AbcGeom::IPointsSchema::Sample previous_sample;
    	points.getSchema().get( previous_sample, previous_sample_selector );

        AlembicPointsDataIndexedMap previous_interim_points;
        build_interim_points_for_arnold_ass(&previous_sample,previous_interim_points);

        // ==============

    	Alembic::Abc::ISampleSelector next_sample_selector(requested_index+1);

        Alembic::AbcGeom::IPointsSchema::Sample next_sample;
    	points.getSchema().get( next_sample, next_sample_selector );

        AlembicPointsDataIndexedMap next_interim_points;
        build_interim_points_for_arnold_ass(&next_sample,next_interim_points);

        // ==================
    	ArnoldPointsData arnold_points_data;
    	build_points_for_arnold_ass_from_interim_points(&previous_interim_points,
    													&current_interim_points,
														&next_interim_points,
														i_relative_shutter_open,
														i_relative_shutter_close,
														i_motion_samples,
														arnold_points_data);
    	// write_arnold_points_data_to_csv_sequence(arnold_points_data,"points_per_sample.%04d.csv");
    	write_arnold_points_data_to_file(arnold_points_data,
    									 i_arnold_filename,
										 i_relative_shutter_open,
										 i_relative_shutter_close);

    }

}

void export_polymesh_as_arnold_ass(Alembic::AbcGeom::IPolyMesh& pmesh,
		   	   	   	   	   	   	   Alembic::Abc::index_t        i_start_frame_number,
								   Alembic::Abc::index_t        i_requested_frame_number,
								   const std::string&           i_arnold_filename,
								   AtByte 						i_motion_samples,
								   float          				i_relative_shutter_open,
								   float          				i_relative_shutter_close
								   )
{

    size_t num_samples = pmesh.getSchema().getNumSamples();
    size_t last_sample_index = num_samples - 1;
    Alembic::Abc::int64_t requested_index = i_requested_frame_number - i_start_frame_number - 1;
    std::cout << boost::format("num_samples = %1% i_requested_frame_number = %2% requested_index = %3% last_sample_index = %4%") % num_samples % i_requested_frame_number % requested_index % last_sample_index << std::endl;
    if (requested_index < 0 || requested_index > (num_samples-1))
    	return;

	Alembic::Abc::ISampleSelector current_sample_selector(requested_index);
    Alembic::AbcGeom::IPolyMeshSchema::Sample current_sample;
	pmesh.getSchema().get( current_sample, current_sample_selector );

    if (requested_index == 0)
    {
    	// First frame
    	Alembic::Abc::ISampleSelector next_sample_selector(requested_index+1);

        Alembic::AbcGeom::IPolyMeshSchema::Sample next_sample;
    	pmesh.getSchema().get( next_sample, next_sample_selector );

    	ArnoldMeshData arnold_mesh_data;
    	build_polymesh_for_arnold_ass(0,
    								  &current_sample,
									  &next_sample,
									  i_relative_shutter_open,
									  i_relative_shutter_close,
									  i_motion_samples,
									  arnold_mesh_data);
    	write_arnold_mesh_data_to_file(arnold_mesh_data,i_arnold_filename,
				  i_relative_shutter_open,
				  i_relative_shutter_close);

    }
    else if (requested_index == last_sample_index)
    {
    	// Last frame
    	Alembic::Abc::ISampleSelector previous_sample_selector(requested_index-1);

        Alembic::AbcGeom::IPolyMeshSchema::Sample previous_sample;
    	pmesh.getSchema().get( previous_sample, previous_sample_selector );

    	ArnoldMeshData arnold_mesh_data;
    	build_polymesh_for_arnold_ass(&previous_sample,
    								  &current_sample,
									  0,
									  i_relative_shutter_open,
									  i_relative_shutter_close,
									  i_motion_samples,
									  arnold_mesh_data);
    	write_arnold_mesh_data_to_file(arnold_mesh_data,i_arnold_filename,
				  i_relative_shutter_open,
				  i_relative_shutter_close);

    }
    else
    {
    	// Frame between first and last frame
    	Alembic::Abc::int64_t previous_index = requested_index - 1;
    	Alembic::Abc::int64_t next_index = requested_index + 1;
    	Alembic::Abc::ISampleSelector previous_sample_selector(previous_index);
    	Alembic::Abc::ISampleSelector next_sample_selector(next_index);
    	std::cout << boost::format("previous_index = %1% requested_index = %2% next_index = %3%") % previous_index % requested_index % next_index << std::endl;
        Alembic::AbcGeom::IPolyMeshSchema::Sample next_sample;
    	pmesh.getSchema().get( next_sample, next_sample_selector );

        Alembic::AbcGeom::IPolyMeshSchema::Sample previous_sample;
    	pmesh.getSchema().get( previous_sample, previous_sample_selector );

    	{
    		print_sample_data("previous",previous_sample);
    		print_sample_data("current",current_sample);
    		print_sample_data("next",next_sample);
    	}
    	ArnoldMeshData arnold_mesh_data;
    	build_polymesh_for_arnold_ass(&previous_sample,
    								  &current_sample,
									  &next_sample,
									  i_relative_shutter_open,
									  i_relative_shutter_close,
									  i_motion_samples,
									  arnold_mesh_data);
    	write_arnold_mesh_data_to_file(arnold_mesh_data,i_arnold_filename,
    								   i_relative_shutter_open,
									   i_relative_shutter_close);
    	write_arnold_mesh_data_to_wavefront_sequence(arnold_mesh_data,"mesh_per_sample.%04d.obj");
    }
}

void flatten_string_array(const StringContainer& i_string_array,
						  const std::string&     i_separator,
						  std::string&           o_flatten_string)
{
	size_t num_strings = i_string_array.size();
	for (size_t index=0;index<num_strings;index++)
	{
		o_flatten_string.append(i_string_array[index]);
		if (index<(num_strings-1)) // We want to avoid a trailing separator!
			o_flatten_string.append(i_separator);
	}
}

void locate_geometry_in_hierarchy(const Alembic::Abc::IObject& top,
								  const StringContainer&       i_hierachy_path,
								  size_t 					  i_requested_index,
								  float          			  i_relative_shutter_open,
								  float          			  i_relative_shutter_close,
								  AtByte						  num_motion_samples,
								  size_t                       i_level = 0)
{
	size_t numChildren = top.getNumChildren();
	std::string unique_object_path;

	for (size_t i=0;i<numChildren;i++)
	{
		std::string child_name =top.getChildHeader(i).getName();
		for (size_t indent=0;indent<i_level;indent++)
			std::cout << "  ";
		Alembic::Abc::IObject child(top,child_name);
		const Alembic::Abc::MetaData &child_md = child.getMetaData();
		std::string metadata_string = child_md.serialize();
		std::cout << boost::format("metadata_string='%1%' child_name='%2%'") % metadata_string % child_name;
		if (Alembic::AbcGeom::IPolyMeshSchema::matches(child_md))
		{
			std::cout << "00000" << std::endl;
	        Alembic::AbcGeom::IPolyMesh mesh(top,child_name);
	        Alembic::AbcGeom::IPolyMeshSchema& schema = mesh.getSchema();

	    	Alembic::Abc::IV3fArrayProperty velocities_property = schema.getVelocitiesProperty();
	    	if (velocities_property.valid())
	    	{
				std::cout << "00100" << std::endl;
	    		StringContainer       _concatenated_hierachy_path = i_hierachy_path;
	    		_concatenated_hierachy_path.push_back(child_name);
	    		flatten_string_array(_concatenated_hierachy_path, "_", unique_object_path);


	    		Alembic::AbcGeom::TimeSamplingPtr ts_ptr = mesh.getSchema().getTimeSampling();
	    		Alembic::AbcGeom::TimeSamplingType timeType = ts_ptr->getTimeSamplingType();
	    		Alembic::AbcGeom::chrono_t tpc = timeType.getTimePerCycle();
	    		// Alembic::AbcGeom::chrono_t fps = 1.0/tpc;
    			// std::cout << boost::format("fps = %1%") % fps << std::endl;
	    		if ( timeType.isUniform() )
	    		{
	    			size_t start_frame = ts_ptr->getStoredTimes()[0] / tpc;
	    			// std::cout << boost::format("start_frame = %1%") % start_frame << std::endl;
	    			std::string arnold_filename = (boost::format("%s.%04d.ass") % unique_object_path % i_requested_index).str();
	    			export_polymesh_as_arnold_ass(mesh,start_frame,i_requested_index,arnold_filename,num_motion_samples,i_relative_shutter_open,i_relative_shutter_close);
	    		}
	    	}
		}
		else if (Alembic::AbcGeom::IPointsSchema::matches(child_md))
		{
			std::cout << " of type Points";
	        Alembic::AbcGeom::IPoints points(top,child_name);
	        Alembic::AbcGeom::IPointsSchema& schema = points.getSchema();

	    	Alembic::Abc::IV3fArrayProperty velocities_property = schema.getVelocitiesProperty();
	    	Alembic::Abc::IUInt64ArrayProperty ids_property = schema.getIdsProperty();
	    	if (velocities_property.valid() && ids_property.valid())
	    	{
	    		StringContainer       _concatenated_hierachy_path = i_hierachy_path;
	    		_concatenated_hierachy_path.push_back(child_name);
	    		flatten_string_array(_concatenated_hierachy_path, "_", unique_object_path);
	    		std::cout << boost::format(" of type Points, unique_object_path : '%1%'") % unique_object_path << std::endl;

	    		Alembic::AbcGeom::TimeSamplingPtr ts_ptr = points.getSchema().getTimeSampling();
	    		Alembic::AbcGeom::TimeSamplingType timeType = ts_ptr->getTimeSamplingType();
	    		Alembic::AbcGeom::chrono_t tpc = timeType.getTimePerCycle();
	    		// Alembic::AbcGeom::chrono_t fps = 1.0/tpc;
    			// std::cout << boost::format("fps = %1%") % fps << std::endl;
	    		if ( timeType.isUniform() )
	    		{
		    		std::cout << " timeType.isUniform() == true" << std::endl;
	    			size_t start_frame = ts_ptr->getStoredTimes()[0] / tpc;
	    			// std::cout << boost::format("start_frame = %1%") % start_frame << std::endl;
	    			std::string arnold_filename = (boost::format("%s.%04d.ass") % unique_object_path % i_requested_index).str();
		    		std::cout << boost::format(" arnold_filename : '%1%'") % arnold_filename << std::endl;
	    			export_points_as_arnold_ass(points,start_frame,i_requested_index,arnold_filename,num_motion_samples,i_relative_shutter_open,i_relative_shutter_close);
	    		}
	    		else if (timeType.isCyclic())
	    		{
		    		std::cout << " timeType.isCyclic() == true" << std::endl;
	    		}
	    		else if (timeType.isAcyclic())
	    		{
		    		std::cout << " timeType.isAcyclic() == true" << std::endl;
	    		}
	    	}
		}
		std::cout << std::endl;
		StringContainer       concatenated_hierachy_path = i_hierachy_path;
		concatenated_hierachy_path.push_back(child_name);
		locate_geometry_in_hierarchy(child,
								concatenated_hierachy_path,
								i_requested_index,
								i_relative_shutter_open,
								i_relative_shutter_close,
								num_motion_samples,
								i_level+1);
	}
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage : " << argv[0] << " <Frame (integer)> <Alembic Archive> <Arnold ASS>" << std::endl;
        return 1;
    }
    size_t frame_to_export(atoi(argv[1]));
    std::string alembic_fileName(argv[2]);
    std::string ass_fileName(argv[3]);
    Alembic::AbcGeom::IArchive alembic_archive;
    alembic_archive = Alembic::AbcGeom::IArchive( Alembic::AbcCoreOgawa::ReadArchive(), alembic_fileName );

	StringContainer       hierachy_path;
	float relative_shutter_open = -0.25f;
	float relative_shutter_close = 0.25f;
	AtByte num_motion_samples = 3;

	locate_geometry_in_hierarchy(alembic_archive.getTop(),hierachy_path,frame_to_export,relative_shutter_open,relative_shutter_close,num_motion_samples);


    return 0;
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
