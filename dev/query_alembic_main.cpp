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

#include <boost/tokenizer.hpp>
#include <boost/format.hpp>

typedef std::vector<std::string> PathList;

void TokenizePath( const std::string &path, PathList &result )
{
    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;

    Tokenizer tokenizer( path, Separator( "/" ) );

    for ( Tokenizer::iterator iter = tokenizer.begin() ; iter != tokenizer.end() ;
          ++iter )
    {
        if ( (*iter).empty() ) { continue; }

        result.push_back( *iter );
    }
}

void arbitrary_geometry_attributes(const Alembic::AbcGeom::ICompoundProperty& arbParams)
{
	size_t num_properties = arbParams.getNumProperties();
	std::cout << boost::format(", user_properties(%1%)") % num_properties;
}

void iterate_hierarchy(const Alembic::Abc::IObject& top, size_t level = 0)
{
	size_t numChildren = top.getNumChildren();

	for (size_t i=0;i<numChildren;i++)
	{
		std::string child_name =top.getChildHeader(i).getName();
		for (size_t indent=0;indent<level;indent++)
			std::cout << "  ";
		std::cout << boost::format("name : %1%") % child_name;
		Alembic::Abc::IObject child(top,child_name);
		const Alembic::Abc::MetaData &child_md = child.getMetaData();
		if (Alembic::AbcGeom::IPolyMeshSchema::matches(child_md))
		{
			std::cout << " of type PolyMesh";
	        Alembic::AbcGeom::IPolyMesh mesh(top,child_name);
	        Alembic::AbcGeom::IPolyMeshSchema& schema = mesh.getSchema();

	        arbitrary_geometry_attributes(schema.getArbGeomParams());
	        size_t num_properties = schema.getNumProperties();
	    	std::cout << boost::format(", geometry_properties(%1%)") % num_properties;
	    	Alembic::Abc::IV3fArrayProperty velocities_property = schema.getVelocitiesProperty();
	    	if (velocities_property.valid())
		    	std::cout << ", has velocity";
	    	else
	    		std::cout << ", does NOT have velocity";
		}
		else if (Alembic::AbcGeom::IPointsSchema::matches(child_md))
		{
			std::cout << " of type Points";
	        Alembic::AbcGeom::IPoints points(top,child_name);
	        Alembic::AbcGeom::IPointsSchema& schema = points.getSchema();

	        arbitrary_geometry_attributes(schema.getArbGeomParams());
		}
		else if (Alembic::AbcGeom::IXformSchema::matches(child_md))
		{
			std::cout << " of type Xform";
		}
		else if (Alembic::AbcGeom::ICurvesSchema::matches(child_md))
		{
			std::cout << " of type Curves";
		}
		std::cout << std::endl;
		iterate_hierarchy(child,level+1);
	}
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage : " << argv[0] << " <Alembic Archive>" << std::endl;
        return 1;
    }
    std::string m_fileName(argv[1]);
    Alembic::AbcGeom::IArchive m_archive;
    m_archive = Alembic::AbcGeom::IArchive( Alembic::AbcCoreOgawa::ReadArchive(),
            m_fileName );
    uint32_t numSamplings = m_archive.getNumTimeSamplings();
    std::cout << "archive_num_samples = " << numSamplings << std::endl;
    if (numSamplings > 1)
    {
        Alembic::Abc::chrono_t min = m_archive.getTimeSampling(1)->getSampleTime(0);
        std::cout << "min time = " << min << std::endl;
        Alembic::Abc::chrono_t max = m_archive.getTimeSampling(1)->getSampleTime(1);
        std::cout << "max time = " << max << std::endl;
        size_t numStoredTimes = m_archive.getTimeSampling(1)->getNumStoredTimes();
        std::cout << "numStoredTimes = " << numStoredTimes << std::endl;
        for (Alembic::Util::uint32_t s = 2; s < numSamplings; ++s)
        {
            Alembic::Abc::chrono_t current_min = m_archive.getTimeSampling(s)->getSampleTime(0);
            std::cout << "current_min = " << current_min << std::endl;

        }

    }
    Alembic::Abc::IObject top = m_archive.getTop();
    iterate_hierarchy(top);

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
