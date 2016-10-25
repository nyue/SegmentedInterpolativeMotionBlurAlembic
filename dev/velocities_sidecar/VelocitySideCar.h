#pragma once

#include <fstream>
#include <vector>
#include <map>
#include <OpenEXR/ImathVec.h>

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

class VelocitySideCar
{
	typedef std::vector<std::string> PathList;
public:
	typedef std::vector<Imath::V3f> V3fContainer;
	typedef std::map<std::string,int> NamedV3fContainerMap;
	std::string _source_sha;
	V3fContainer _v;
	NamedV3fContainerMap _velocities;
public:
	VelocitySideCar();
	virtual ~VelocitySideCar();
	const std::string& get_sha() const;
	bool compute(const std::string& i_alembic_source, const std::string& i_regex_pattern = "*");
protected:
    virtual void ProcessIPolyMesh(const Alembic::AbcGeom::IPolyMesh& i_polymesh,
    							  const std::string&                 i_regex_pattern);
private:
	void iterate_iobject(Alembic::Abc::IObject             parent,
						 const Alembic::Abc::ObjectHeader& ohead,
						 const std::string&                i_regex_pattern,
						 PathList::const_iterator          I,
						 PathList::const_iterator          E);
	void TokenizePath( const std::string &path, PathList& result ) const;
};

void compute_sha(const std::string& i_filename, std::string& o_sha);

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
