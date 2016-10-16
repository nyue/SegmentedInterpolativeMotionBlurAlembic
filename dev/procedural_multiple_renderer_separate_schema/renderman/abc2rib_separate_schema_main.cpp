#include "RendermanPointsSchemaHandler.h"
#include "RendermanPolyMeshSchemaHandler.h"
#include <String2ArgcArgv.h>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <stdio.h> // required when building as procedural "RunProgram"
#include <glog/logging.h>

namespace po = boost::program_options;

/*!
 * \todo Remove the i_level param as this is a procedural
 */
void locate_geometry_in_hierarchy(const Alembic::Abc::IObject& top,
								  const StringContainer&       i_hierachy_path,
								  size_t 					   i_requested_index,
								  float          			   i_relative_shutter_open,
								  float          			   i_relative_shutter_close,
								  Alembic::Abc::uint8_t        num_motion_samples,
								  size_t                       i_level = 0)
{
	size_t numChildren = top.getNumChildren();
	std::string unique_object_path;

	for (size_t i=0;i<numChildren;i++)
	{
		std::string child_name =top.getChildHeader(i).getName();
		for (size_t indent=0;indent<i_level;indent++)
			DLOG(INFO) << "  ";
		Alembic::Abc::IObject child(top,child_name);
		const Alembic::Abc::MetaData &child_md = child.getMetaData();
		std::string metadata_string = child_md.serialize();
		DLOG(INFO) << boost::format("metadata_string='%1%' child_name='%2%'") % metadata_string % child_name;
		if (Alembic::AbcGeom::IPolyMeshSchema::matches(child_md))
		{
			RendermanPolyMeshSchemaHandler apmsh;

			apmsh.ProcessPolyMesh(top, child_name, i_hierachy_path, i_requested_index, i_relative_shutter_open, i_relative_shutter_close, num_motion_samples);
		}
		else if (Alembic::AbcGeom::IPointsSchema::matches(child_md))
		{
			RendermanPointsSchemaHandler apsh;
			apsh.ProcessPoints(top, child_name, i_hierachy_path, i_requested_index, i_relative_shutter_open, i_relative_shutter_close, num_motion_samples);
		}
		DLOG(INFO) << std::endl;
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

int EmitGeometry(int argc, const char** argv)
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
		locate_geometry_in_hierarchy(alembic_archive.getTop(),hierachy_path,frame_to_export,relative_shutter_open,relative_shutter_close,num_motion_samples);
	}
	catch(std::exception& e) {
		LOG(ERROR) << "error: " << e.what() << "\n";
		return 1;
	}
	catch(...) {
		LOG(ERROR) << "Exception of unknown type!\n";
	}
	return 0;
}

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

RtPointer ConvertParameters(RtString paramstr)
{
	std::string param_std_string = (boost::format("dummy %1%") % paramstr).str();
	DLOG(INFO) << boost::format("param_std_string = '%1%'") % param_std_string << std::endl;
	PI::String2ArgcArgv* s2aa = new PI::String2ArgcArgv(param_std_string);

	/* return the blind data pointer */
	return (RtPointer)s2aa;
}

RtVoid Subdivide(RtPointer data, RtFloat detail)
{
	PI::String2ArgcArgv* s2aa = (PI::String2ArgcArgv *)data;

	EmitGeometry(s2aa->argc(), s2aa->argv());

}

RtVoid Free(RtPointer data)
{
	PI::String2ArgcArgv* s2aa = (PI::String2ArgcArgv *)data;
	delete s2aa;
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/*!
 * \note This should be harmless when compiled as a procedural "DynamicLoad" but will
 *       be the entry point for procedural "RunProgram"
 */
int main(int argc, char** argv)
{
	google::InitGoogleLogging(argv[0]);
	RtFloat normalize_pixel_coverage;
	DLOG(INFO) << boost::format("normalize_pixel_coverage = %1%") % normalize_pixel_coverage;
	char  args[BUFSIZ];

	while(fgets(args,BUFSIZ,stdin) != NULL) {
		std::string args_string(args);
		args_string.erase(std::remove(args_string.begin(), args_string.end(), '\n'), args_string.end());
		DLOG(INFO) << boost::format("args_string = '%1%'") % args_string << std::endl;
		// std::cerr << boost::format("args = '%1%'") % args << std::endl;
		PI::String2ArgcArgv s2aa(args_string);
		sscanf(args, "%g", &normalize_pixel_coverage);
		RiBegin("stdout");
		RiAttributeBegin();
		EmitGeometry(s2aa.argc(), s2aa.argv());
		RiAttributeEnd();
		RiArchiveRecord(RI_VERBATIM, "\377");
		RiEnd();
	}
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
