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

typedef std::vector<Imath::V3f> V3fContainer;
struct WavefrontMeshData
{
	V3fContainer _positions;
	V3fContainer _normals;
};
typedef std::vector<WavefrontMeshData> WavefrontMeshDataContainer;
typedef std::vector<std::string> StringContainer;

void export_polymesh_as_wavefront_obj(Alembic::AbcGeom::IPolyMesh&         pmesh,
									  const Alembic::Abc::ISampleSelector& i_sample_selector,
									  const std::string&                   i_wavefront_filename)
{
    Alembic::AbcGeom::IPolyMeshSchema::Sample samp;

    if ( pmesh.getSchema().getNumSamples() > 0 )
    {

    	std::ofstream wavefront_file;
    	wavefront_file.open (i_wavefront_filename.c_str());

    	wavefront_file << "# File exported by Nicholas Yue\n";


    	pmesh.getSchema().get( samp, i_sample_selector );

    	Alembic::AbcGeom::P3fArraySamplePtr P = samp.getPositions();
    	Alembic::AbcGeom::Int32ArraySamplePtr indices = samp.getFaceIndices();
    	Alembic::AbcGeom::Int32ArraySamplePtr counts = samp.getFaceCounts();
    	Alembic::AbcGeom::V3fArraySamplePtr v = samp.getVelocities();

        size_t P_size = P->size();
        size_t indices_size = indices->size();
        size_t counts_size = counts->size();
        size_t v_size = v->size();

        wavefront_file << boost::format("# %1% points") % P_size << std::endl;
        wavefront_file << boost::format("# %1% velocities") % v_size << std::endl;
        wavefront_file << boost::format("# %1% faces") % counts_size << std::endl;

        wavefront_file << "g" << std::endl;
        for (size_t index=0;index<P_size;index++)
        {
            wavefront_file << boost::format("v %1% %2% %3%") % P->get()[index].x % P->get()[index].y % P->get()[index].z << std::endl;
        }
        for (size_t index=0;index<v_size;index++)
        {
            wavefront_file << boost::format("vn %1% %2% %3%") % v->get()[index].x % v->get()[index].y % v->get()[index].z << std::endl;
        }

        wavefront_file << "g" << std::endl;
        size_t indices_index = 0;
        for (size_t index=0;index<counts_size;index++)
        {
            wavefront_file << "f";
        	size_t face_vert_count = counts->get()[index];
        	for (size_t face_vert_index=0;face_vert_index < face_vert_count; ++face_vert_index)
        	{
        		size_t face_vert_index_value = indices->get()[indices_index] + 1; // OBJ array is not zero based!
        		wavefront_file << boost::format(" %1%//%1%") % face_vert_index_value;
        		indices_index++;
        	}
    		wavefront_file << std::endl;
        }
        wavefront_file.close();
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
							 WavefrontMeshDataContainer&  o_meshes,
							 size_t                       i_level = 0)
{
	size_t numChildren = top.getNumChildren();
	std::string unique_object_path;

	for (size_t i=0;i<numChildren;i++)
	{
		std::string child_name =top.getChildHeader(i).getName();
		for (size_t indent=0;indent<i_level;indent++)
			std::cout << "  ";
		std::cout << boost::format("name : %1%") % child_name;
		Alembic::Abc::IObject child(top,child_name);
		const Alembic::Abc::MetaData &child_md = child.getMetaData();
		if (Alembic::AbcGeom::IPolyMeshSchema::matches(child_md))
		{
	        Alembic::AbcGeom::IPolyMesh mesh(top,child_name);
	        Alembic::AbcGeom::IPolyMeshSchema& schema = mesh.getSchema();

	    	Alembic::Abc::IV3fArrayProperty velocities_property = schema.getVelocitiesProperty();
	    	if (velocities_property.valid())
	    	{
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
	    	    size_t num_samples = mesh.getSchema().getNumSamples();
	    	    // std::cout << boost::format("num_samples = %1%") % num_samples << std::endl;

	    	    for (Alembic::Abc::index_t sample_index = 0;sample_index<num_samples;++sample_index)
	    	    {
	    	    	Alembic::Abc::ISampleSelector iSS(sample_index);
		    		// std::cout << boost::format(" of type PolyMesh, unique_object_path : '%1%'") % unique_object_path << std::endl;
		    		std::string wavefront_filename = (boost::format("%s.%04d.obj") % unique_object_path % sample_index).str();
		    		export_polymesh_as_wavefront_obj(mesh,iSS,wavefront_filename);
	    	    }
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
		get_mesh_from_hierarchy(child,
								concatenated_hierachy_path,
								o_meshes,
								i_level+1);
	}
}

void get_points_positions_velocities(Alembic::AbcGeom::IArchive& i_alembic_archive,
									 const std::string           i_wavefront_basedir,
									 WavefrontMeshDataContainer& o_meshes)
{
    Alembic::Abc::IObject top = i_alembic_archive.getTop();
	size_t numChildren = top.getNumChildren();
	std::cout << boost::format("numChildren = %1%") % numChildren << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage : " << argv[0] << " <Alembic Archive> <Wavefront OBJ>" << std::endl;
        return 1;
    }
    std::string alembic_fileName(argv[1]);
    std::string wavefront_fileName(argv[2]);
    Alembic::AbcGeom::IArchive alembic_archive;
    alembic_archive = Alembic::AbcGeom::IArchive( Alembic::AbcCoreOgawa::ReadArchive(), alembic_fileName );

    WavefrontMeshDataContainer meshes;
	StringContainer       hierachy_path;

	uint32_t NumTimeSamplings = alembic_archive.getNumTimeSamplings();
	for (uint32_t samplingIndex=0;samplingIndex<NumTimeSamplings;samplingIndex++)
	{
		Alembic::AbcGeom::TimeSamplingPtr ts_ptr = alembic_archive.getTimeSampling(samplingIndex);
		Alembic::AbcGeom::TimeSamplingType ts_type = ts_ptr->getTimeSamplingType();
	    std::cout << boost::format("ts[%1%] : isAcyclic = %2% isCyclic = %3% isUniform = %4% samples_per_cycle = %5%")
	    							% samplingIndex
									% ts_type.isAcyclic()
									% ts_type.isCyclic()
									% ts_type.isUniform()
									% ts_type.getNumSamplesPerCycle()
									<< std::endl;
	}
    std::cout << boost::format("NumTimeSamplings = %1%") % NumTimeSamplings << std::endl;
	get_mesh_from_hierarchy(alembic_archive.getTop(),hierachy_path,meshes);

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
