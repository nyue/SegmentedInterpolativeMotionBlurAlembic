#include "VelocitySideCar.h"
#include <boost/tokenizer.hpp>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>

VelocitySideCar::VelocitySideCar()
{
}

VelocitySideCar::~VelocitySideCar()
{
}

const std::string& VelocitySideCar::get_sha() const
{
	return _source_sha;
};

bool VelocitySideCar::compute(const std::string& i_alembic_source, const std::string& i_regex_pattern)
{
	compute_sha(i_alembic_source, _source_sha);

	Alembic::AbcCoreFactory::IFactory factory;
	Alembic::AbcCoreFactory::IFactory::CoreType oType;
	Alembic::Abc::IArchive archive(factory.getArchive(i_alembic_source, oType));


    Alembic::Abc::IObject top = archive.getTop();

    size_t top_num_children = top.getNumChildren();
    std::cout << "top_num_children = " << top_num_children << std::endl;
    std::string top_name = top.getName();
    std::cout << "top_name = " << top_name << std::endl;

    PathList path;
    TokenizePath( std::string("/grid_object1/grid1"), path );
    PathList::const_iterator I = path.begin();
    const Alembic::Abc::ObjectHeader *nextChildHeader = top.getChildHeader (*I);
    for ( size_t i = 0; i < top.getNumChildren(); ++i )
    {
        iterate_iobject( top, top.getChildHeader(i),i_regex_pattern, I+1,path.end());
    }
	return true;
}

void VelocitySideCar::ProcessIPolyMesh(const Alembic::AbcGeom::IPolyMesh& i_polymesh,
		  	  	  	  	  	  	  	   const std::string&                 i_regex_pattern)
{

}

void VelocitySideCar::iterate_iobject(Alembic::Abc::IObject             parent,
									  const Alembic::Abc::ObjectHeader& ohead,
									  const std::string&                i_regex_pattern,
									  PathList::const_iterator          I,
									  PathList::const_iterator          E)
{
    std::cout << "iterate_iobject ohead's name = " << ohead.getName().c_str() << std::endl;
    std::cout << "iterate_iobject ohead's full name = " << ohead.getFullName().c_str() << std::endl;

    //set this if we should continue traversing
    Alembic::Abc::IObject nextParentObject;

    if ( Alembic::AbcGeom::IXform::matches( ohead ) )
    {
        std::cout << "iterate_iobject match IXform" << std::endl;
        Alembic::AbcGeom::IXform xform( parent, ohead.getName() );

        nextParentObject = xform;

    }
    else if ( Alembic::AbcGeom::ISubD::matches( ohead ) )
    {
        std::cout << "iterate_iobject match ISubD" << std::endl;
        Alembic::AbcGeom::ISubD subd( parent, ohead.getName() );
        nextParentObject = subd;

    }
    else if ( Alembic::AbcGeom::IPolyMesh::matches( ohead ) )
    {
        std::cout << "iterate_iobject match IPolyMesh" << std::endl;
        Alembic::AbcGeom::IPolyMesh polymesh( parent, ohead.getName() );
        ProcessIPolyMesh(polymesh, i_regex_pattern);
        nextParentObject = polymesh;

    }
    else if ( Alembic::AbcGeom::INuPatch::matches( ohead ) )
    {
        std::cout << "iterate_iobject match INuPatch" << std::endl;

    }
    else if ( Alembic::AbcGeom::IPoints::matches( ohead ) )
    {
        std::cout << "iterate_iobject match IPoints" << std::endl;
        Alembic::AbcGeom::IPoints points( parent, ohead.getName() );
        // ProcessIPoints(points);
        nextParentObject = points;
    }
    else if ( Alembic::AbcGeom::ICurves::matches( ohead ) )
    {
        std::cout << "iterate_iobject match ICurves" << std::endl;

    }
    else if ( Alembic::AbcGeom::IFaceSet::matches( ohead ) )
    {
        std::cout << "iterate_iobject match IFaceSet" << std::endl;
        std::cerr << "DOH !" << std::endl;
    }

    // Recursion
    if ( nextParentObject.valid() )
    {
        for ( size_t i = 0; i < nextParentObject.getNumChildren() ; ++i )
        {
            iterate_iobject( nextParentObject, nextParentObject.getChildHeader( i ), i_regex_pattern, I, E);
        }
    }
}

void VelocitySideCar::TokenizePath( const std::string &path, PathList& result ) const
{
    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;

    Tokenizer tokenizer( path, Separator( "/" ) );

    for ( Tokenizer::iterator iter = tokenizer.begin() ; iter != tokenizer.end() ;
          ++iter )
    {
		  if ( (*iter).empty() )
		  {
			  continue;
		  }
		  std::cout << "*iter = " << *iter << std::endl;
		  result.push_back( *iter );
    }
}

void compute_sha(const std::string& i_filename, std::string& o_sha)
{
	CryptoPP::SHA1 hash;
	CryptoPP::FileSource(i_filename.c_str(),true,
			new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(o_sha), true)));
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
