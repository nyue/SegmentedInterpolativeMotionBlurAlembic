#include "ArnoldPointsSchemaHandler.h"
#include "ArnoldPolyMeshSchemaHandler.h"
#include "ProcArgs.h"
#include <String2ArgcArgv.h>
#include <logging.h>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

void locate_geometry_in_hierarchy(const Alembic::Abc::IObject& top,
								  const StringContainer&       i_hierachy_path,
								  size_t 					   i_requested_index,
								  float          			   i_relative_shutter_open,
								  float          			   i_relative_shutter_close,
								  Alembic::Abc::uint8_t        num_motion_samples,
								  AtNodePtrContainer&          o_created_nodes)
{
	size_t numChildren = top.getNumChildren();
	std::string unique_object_path;

	for (size_t i=0;i<numChildren;i++)
	{
		std::string child_name =top.getChildHeader(i).getName();
		Alembic::Abc::IObject child(top,child_name);
		const Alembic::Abc::MetaData &child_md = child.getMetaData();
		std::string metadata_string = child_md.serialize();
		if (Alembic::AbcGeom::IPolyMeshSchema::matches(child_md))
		{
			ArnoldPolyMeshSchemaHandler apmsh;

			apmsh.set_created_nodes_container(&o_created_nodes);
			apmsh.ProcessPolyMesh(top, child_name, i_hierachy_path, i_requested_index, i_relative_shutter_open, i_relative_shutter_close, num_motion_samples);
		}
		else if (Alembic::AbcGeom::IPointsSchema::matches(child_md))
		{
			ArnoldPointsSchemaHandler apsh;

			apsh.set_created_nodes_container(&o_created_nodes);
			apsh.ProcessPoints(top, child_name, i_hierachy_path, i_requested_index, i_relative_shutter_open, i_relative_shutter_close, num_motion_samples);
		}
		StringContainer       concatenated_hierachy_path = i_hierachy_path;
		concatenated_hierachy_path.push_back(child_name);
		locate_geometry_in_hierarchy(child,
								concatenated_hierachy_path,
								i_requested_index,
								i_relative_shutter_open,
								i_relative_shutter_close,
								num_motion_samples,
								o_created_nodes);
	}
}

int EmitGeometry(int argc, const char** argv, AtNodePtrContainer& o_created_nodes)
{
	try {

	    size_t                frame_to_export = 1;
	    std::string           alembic_fileName;
	    std::string           velocity_fileName;
	    std::string           geometry_filter;
		float                 relative_shutter_open = -0.25f;
		float                 relative_shutter_close = 0.25f;
		size_t                num_motion_samples = 3;

		po::options_description desc("Allowed options");
		desc.add_options()
    		("version", "print version string")
    		("help", "produce help message")
    		("samples,s", po::value<size_t>(&num_motion_samples),
    				"number of samples to generate [default 3]")
    		("open,o", po::value<float>(&relative_shutter_open),
    				"relative shutter open [default -0.25]")
    		("close,c", po::value<float>(&relative_shutter_close),
    				"relative shutter close [default 0.25]")
    		("frame,f", po::value<size_t>(&frame_to_export),
    				"frame to export [default 1]")
    		("abc,a", po::value<std::string>(&alembic_fileName),
    				"alembic file")
			("velocity,v", po::value<std::string>(&velocity_fileName),
					"velocity file")
    				;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help") || alembic_fileName.empty()) {
			std::cout << desc << "\n";
			return 1;
		}


		Alembic::AbcCoreFactory::IFactory factory;
		Alembic::AbcCoreFactory::IFactory::CoreType oType;

	    Alembic::AbcGeom::IArchive alembic_archive(factory.getArchive(alembic_fileName, oType));

		StringContainer       hierachy_path;
		locate_geometry_in_hierarchy(alembic_archive.getTop(),hierachy_path,frame_to_export,relative_shutter_open,relative_shutter_close,num_motion_samples,o_created_nodes);
	}
	catch(std::exception& e) {
		LOG(ERROR) << "error: " << e.what() << "\n";
		return 1;
	}
	catch(...) {
		LOG(ERROR) << "Exception of unknown type!\n";
	}
	return 0;
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
	Alembic::Abc::uint8_t num_motion_samples = 3;

	locate_geometry_in_hierarchy(alembic_archive.getTop(),hierachy_path,frame_to_export,relative_shutter_open,relative_shutter_close,num_motion_samples,o_created_nodes);


    return 0;
}

int ProcInit( struct AtNode *node, void **user_ptr )
{
	google::InitGoogleLogging("ProcInit");
    ProcArgs * args = new ProcArgs();
    args->proceduralNode = node;

    const char *parentProceduralDATA = AiNodeGetStr(node,"data");
    std::cout << boost::format("parentProceduralDATA = '%1%'") % parentProceduralDATA << std::endl;

	std::string param_std_string = (boost::format("dummy %1%") % parentProceduralDATA).str();
	DLOG(INFO) << boost::format("param_std_string = '%1%'") % param_std_string << std::endl;
	PI::String2ArgcArgv s2aa(param_std_string);

	EmitGeometry(s2aa.argc(), s2aa.argv(),args->createdNodes);

	if (false){
		// Do stuff here
		args->createdNodes.push_back(AiNode("sphere"));
	}

    *user_ptr = args;

	return true;
}

int ProcCleanup( void *user_ptr )
{
	delete reinterpret_cast<ProcArgs*>( user_ptr );

	return true;
}

int ProcNumNodes( void *user_ptr )
{
    ProcArgs * args = reinterpret_cast<ProcArgs*>( user_ptr );
    return (int) args->createdNodes.size();
}

struct AtNode* ProcGetNode(void *user_ptr, int i)
{
    ProcArgs * args = reinterpret_cast<ProcArgs*>( user_ptr );

    if ( i >= 0 && i < (int) args->createdNodes.size() )
    {
        return args->createdNodes[i];
    }

    return NULL;
}

extern "C"
{
    int ProcLoader(AtProcVtable* api)
    {
        api->Init        = ProcInit;
        api->Cleanup     = ProcCleanup;
        api->NumNodes    = ProcNumNodes;
        api->GetNode     = ProcGetNode;
        strcpy(api->version, AI_VERSION);
        return 1;
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
