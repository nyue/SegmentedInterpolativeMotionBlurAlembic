#include "ArnoldUtils.h"
#include "Utils.h"
#include <boost/format.hpp>
#include <fstream>
#include <ai.h>

// =============================================================================
// Arnold PolyMesh
// =============================================================================

struct AtNode * create_arnold_polymesh_node(const std::string&    name,
										   const ArnoldMeshData& i_arnold_mesh_data,
										   float                 i_shutter_open,
										   float                 i_shutter_close)
{
	AtByte nkeys = i_arnold_mesh_data._vlist_data_array.size();
	if (nkeys<1)
		return NULL;
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

	return polymesh;
}

void emit_arnold_mesh_data(const ArnoldMeshData& i_arnold_mesh_data,
						   float 				 i_shutter_open,
						   float 				 i_shutter_close,
							 AtNodePtrContainer&     o_created_nodes,
						   const char*           i_arnold_filename)
{
	if (i_arnold_filename)
		// start an Arnold session
		AiBegin();

	struct AtNode* node = create_arnold_polymesh_node("test",i_arnold_mesh_data,i_shutter_open,i_shutter_close);
	o_created_nodes.push_back(node);

	if (i_arnold_filename)
	{
		int write_mask = AI_NODE_ALL;
		bool is_open_procs = false;
		bool is_binary = false;
		// Write out an .ass file
		AiASSWrite(i_arnold_filename, write_mask, is_open_procs, is_binary);

		// at this point we can shut down Arnold
		AiEnd();
	}
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

// =============================================================================
// Arnold Points
// =============================================================================

struct AtNode* create_arnold_points_node(const std::string&      name,
										const ArnoldPointsData& i_arnold_points_data,
										float                   i_shutter_open,
										float                   i_shutter_close)
{
	AtByte nkeys = i_arnold_points_data._points_data_array.size();
	if (nkeys<1)
		return NULL;
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

	return points;
}

void emit_arnold_points_data(const ArnoldPointsData& i_arnold_points_data,
							 float 				     i_shutter_open,
							 float 				     i_shutter_close,
							 AtNodePtrContainer&     o_created_nodes,
							 const char*             i_arnold_filename)
{
	if (i_arnold_filename)
		// start an Arnold session
		AiBegin();


	struct AtNode* node = create_arnold_points_node("test",i_arnold_points_data,i_shutter_open,i_shutter_close);
	o_created_nodes.push_back(node);

	if (i_arnold_filename)
	{
		// ... or you can write out an .ass file instead
		AiASSWrite(i_arnold_filename, AI_NODE_ALL, false, false);

		// at this point we can shut down Arnold
		AiEnd();
	}

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
