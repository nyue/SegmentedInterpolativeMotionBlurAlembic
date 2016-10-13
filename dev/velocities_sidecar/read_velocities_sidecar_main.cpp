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
#include <boost/format.hpp>
#include <OpenEXR/ImathVec.h>

#include "VelocitySideCar.h"

int main(int argc, char** argv)
{
    std::string filename("non-intrusive_serialization_filename");
    std::ifstream ifs(filename.c_str());

    VelocitySideCar vsc;
    {
    	boost::archive::binary_iarchive ia(ifs);

    	ia >> vsc;
    }

    	return 0;
}
