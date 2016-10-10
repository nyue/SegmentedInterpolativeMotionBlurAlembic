#include "RendermanPointsSchemaHandler.h"
#include "RendermanPolyMeshSchemaHandler.h"

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
			std::cout << "  ";
		Alembic::Abc::IObject child(top,child_name);
		const Alembic::Abc::MetaData &child_md = child.getMetaData();
		std::string metadata_string = child_md.serialize();
		std::cout << boost::format("metadata_string='%1%' child_name='%2%'") % metadata_string % child_name;
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
        std::cerr << "Usage : " << argv[0] << " <Frame (integer)> <Alembic Archive> <Renderman RIB>" << std::endl;
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
	Alembic::Abc::uint8_t num_motion_samples = 15;

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
