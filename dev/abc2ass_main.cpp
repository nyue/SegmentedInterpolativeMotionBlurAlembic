#include <ai.h>

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
#include <iostream>
#include <fstream>

#include <boost/format.hpp>

#include <OpenEXR/ImathVec.h>

template <typename T> void interpolate(const Imath::Vec3<T>& P1,
									   const Imath::Vec3<T>& T1,
									   const Imath::Vec3<T>& P2,
									   const Imath::Vec3<T>& T2,
									   T s,
									   Imath::Vec3<T>& P)
{
	T h1 =  2.0*std::pow(s,3.0) - 3.0*std::pow(s,2.0) + 1.0;          // calculate basis function 1
	T h2 = -2.0*std::pow(s,3.0) + 3.0*std::pow(s,2.0);              // calculate basis function 2
	T h3 =   std::pow(s,3.0) - 2.0*std::pow(s,2.0) + s;         // calculate basis function 3
	T h4 =   std::pow(s,3.0) -  std::pow(s,2.0);              // calculate basis function 4


	P = h1*P1 +                    // multiply and sum all funtions
			h2*P2 +                    // together to build the interpolated
			h3*T1 +                    // point along the curve.
			h4*T2;
}

typedef std::vector<AtUInt32> AtUInt32Container;
typedef std::vector<Imath::V3f> V3fContainer;
typedef std::vector<V3fContainer> V3fContainerArray;
typedef std::vector<std::string> StringContainer;
struct ArnoldMeshData
{
	V3fContainerArray _vlist_data_array;
	// Topological stable being assumed
	AtUInt32Container _nsides_data;
	AtUInt32Container _vidxs_data;
};

void make_arnold_polymesh(const std::string& name,
		const ArnoldMeshData i_arnold_mesh_data)
{
	AtByte nkeys = i_arnold_mesh_data._vlist_data_array.size();
	if (nkeys<1)
		return;
	// Assumes topological stability
	AtUInt32 nelements = i_arnold_mesh_data._vlist_data_array[0].size();
	// name
	AtNode *polymesh = AiNode("polymesh");
	AiNodeSetStr(polymesh, "name", name.c_str());

	// vlist
	AiNodeSetArray(polymesh, "vlist", AiArrayConvert(nelements,nkeys,AI_TYPE_POINT,i_arnold_mesh_data._vlist_data_array.data()));

	// nsides
	AiNodeSetArray(polymesh, "nsides", AiArrayConvert(i_arnold_mesh_data._nsides_data.size(),1,AI_TYPE_UINT,i_arnold_mesh_data._nsides_data.data()));

	// vidxs
	AiNodeSetArray(polymesh, "vidxs", AiArrayConvert(i_arnold_mesh_data._vidxs_data.size(),1,AI_TYPE_UINT,i_arnold_mesh_data._vidxs_data.data()));

}

void write_arnold_mesh_data_to_file(const ArnoldMeshData& i_arnold_mesh_data,
									const std::string&    i_arnold_filename)
{
	  // start an Arnold session
	  AiBegin();
	  AiMsgSetLogFileName("pm.log");

	  make_arnold_polymesh("test",i_arnold_mesh_data);

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
	  AiASSWrite(i_arnold_filename.c_str(), AI_NODE_ALL, false);

	  // at this point we can shut down Arnold
	  AiEnd();

}

/*!
 * \param i_relative_shutter_open This can be negative (relative to the current frame)
 * \param i_relative_shutter_close This can be negative (relative to the current frame)
 */
void build_polymesh_for_arnold_ass(const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_previous_sample,
								   const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_current_sample,
								   const Alembic::AbcGeom::IPolyMeshSchema::Sample* i_next_sample,
								   float											i_relative_shutter_open,
								   float											i_relative_shutter_close,
								   AtByte											i_motion_samples,
								   ArnoldMeshData&                                  o_arnold_mesh)
{
	if (!i_current_sample)
		return;

	// Assumes topologically stable
	Alembic::AbcGeom::Int32ArraySamplePtr indices = i_current_sample->getFaceIndices();
	Alembic::AbcGeom::Int32ArraySamplePtr counts = i_current_sample->getFaceCounts();

	// Current
	Alembic::AbcGeom::P3fArraySamplePtr current_P = i_current_sample->getPositions();
	Alembic::AbcGeom::V3fArraySamplePtr current_v = i_current_sample->getVelocities();

	if ( i_motion_samples == 1)
	{
		// Special case, return single time step from current sample
		size_t num_nsides = counts->size();
		size_t num_indices = indices->size();
		size_t num_P = current_P->size();

		o_arnold_mesh._nsides_data.resize(num_nsides);
		o_arnold_mesh._vidxs_data.resize(num_indices);
		o_arnold_mesh._vlist_data_array.resize(i_motion_samples);
		o_arnold_mesh._vlist_data_array[0].resize(num_P);
		for (size_t index=0;index<num_nsides;index++)
		{
			o_arnold_mesh._nsides_data[index] = counts->get()[index];
		}
		for (size_t index=0;index<num_indices;index++)
		{
			o_arnold_mesh._vidxs_data[index] = indices->get()[index];
		}
		for (size_t index=0;index<num_P;index++)
		{
			o_arnold_mesh._vlist_data_array[0][index].x = current_P->get()[index].x;
			o_arnold_mesh._vlist_data_array[0][index].y = current_P->get()[index].y;
			o_arnold_mesh._vlist_data_array[0][index].z = current_P->get()[index].z;
		}

		return;
	}

	if (i_previous_sample && i_next_sample)
	{

	}
}

void export_polymesh_as_arnold_ass(Alembic::AbcGeom::IPolyMesh& pmesh,
								   Alembic::Abc::index_t        i_requested_index,
								   const std::string&           i_arnold_filename,
								   AtByte 						i_motion_samples)
{
	Alembic::Abc::ISampleSelector current_sample_selector(i_requested_index);
    Alembic::AbcGeom::IPolyMeshSchema::Sample current_sample;
	pmesh.getSchema().get( current_sample, current_sample_selector );

    size_t num_samples = pmesh.getSchema().getNumSamples();
    size_t last_sample_index = num_samples - 1;
    std::cout << boost::format("num_samples = %1%") % num_samples << std::endl;
    if (i_requested_index < 0 || i_requested_index > (num_samples-1))
    	return;

    if (i_requested_index == 0)
    {
    	// First frame
    	Alembic::Abc::ISampleSelector next_sample_selector(i_requested_index+1);

        Alembic::AbcGeom::IPolyMeshSchema::Sample next_sample;
    	pmesh.getSchema().get( next_sample, next_sample_selector );


    }
    else if (i_requested_index == last_sample_index)
    {
    	// Last frame
    	Alembic::Abc::ISampleSelector previous_sample_selector(i_requested_index-1);

        Alembic::AbcGeom::IPolyMeshSchema::Sample previous_sample;
    	pmesh.getSchema().get( previous_sample, previous_sample_selector );

    }
    else
    {
    	// Frame between first and last frame
    	Alembic::Abc::ISampleSelector previous_sample_selector(i_requested_index-1);
    	Alembic::Abc::ISampleSelector next_sample_selector(i_requested_index+1);

        Alembic::AbcGeom::IPolyMeshSchema::Sample previous_sample;
    	pmesh.getSchema().get( previous_sample, previous_sample_selector );

        Alembic::AbcGeom::IPolyMeshSchema::Sample next_sample;
    	pmesh.getSchema().get( next_sample, next_sample_selector );

    	ArnoldMeshData arnold_mesh_data;
    	float          relative_shutter_open = -0.5f;
		float          relative_shutter_close = 0.5f;
    	build_polymesh_for_arnold_ass(&previous_sample,
    								  &current_sample,
									  &next_sample,
									  relative_shutter_open,
									  relative_shutter_close,
									  i_motion_samples,
									  arnold_mesh_data);
    	write_arnold_mesh_data_to_file(arnold_mesh_data,i_arnold_filename);
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

void get_mesh_from_hierarchy(const Alembic::Abc::IObject& top,
							 const StringContainer&       i_hierachy_path,
							 size_t 					  i_requested_index,
//							 WavefrontMeshDataContainer&  o_meshes,
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
	    		Alembic::AbcGeom::chrono_t fps = 1.0/tpc;
	    		if ( timeType.isUniform() ) {
	    			double start_frame = ts_ptr->getStoredTimes()[0] * fps;
		    	    // std::cout << boost::format("start_frame = %1%") % start_frame << std::endl;
	    		}
	    		std::string arnold_filename = (boost::format("%s.%04d.ass") % unique_object_path % i_requested_index).str();
	    		AtByte num_motion_samples = 1;
	    	    export_polymesh_as_arnold_ass(mesh,i_requested_index,arnold_filename,num_motion_samples);
	    	}
		}
		else if (Alembic::AbcGeom::IPointsSchema::matches(child_md))
		{
			std::cout << " of type Points";
	        Alembic::AbcGeom::IPoints points(top,child_name);
	        Alembic::AbcGeom::IPointsSchema& schema = points.getSchema();

	    	Alembic::Abc::IV3fArrayProperty velocities_property = schema.getVelocitiesProperty();
	    	if (velocities_property.valid())
	    	{
	    		StringContainer       _concatenated_hierachy_path = i_hierachy_path;
	    		_concatenated_hierachy_path.push_back(child_name);
	    		flatten_string_array(_concatenated_hierachy_path, "_", unique_object_path);
	    		std::cout << boost::format(" of type Points, unique_object_path : '%1%'") % unique_object_path << std::endl;
	    	}
		}
		std::cout << std::endl;
		StringContainer       concatenated_hierachy_path = i_hierachy_path;
		concatenated_hierachy_path.push_back(child_name);
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

	get_mesh_from_hierarchy(alembic_archive.getTop(),hierachy_path,frame_to_export);


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
