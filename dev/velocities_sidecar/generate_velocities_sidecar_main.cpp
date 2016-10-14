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
#include <boost/program_options.hpp>
#include <OpenEXR/ImathVec.h>

#include "VelocitySideCar.h"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
	try {
		std::string alembic_file;
		std::string velocity_file;
		po::options_description desc("Allowed options");
		desc.add_options()
    		("help", "produce help message")
    		("abc", po::value<std::string>(&alembic_file),
    				"name of alembic file")
			("vsc", po::value<std::string>(&velocity_file),
					"name of velocity file")
    				;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 1;
		}
		if (vm.count("abc")) {
			std::cout << "alembic_file variable " << alembic_file.c_str() << std::endl;
		}
		if (vm.count("vsc")) {
			std::cout << "velocity_file variable " << velocity_file.c_str() << std::endl;
		}
	}
	catch(std::exception& e) {
		std::cerr << "error: " << e.what() << "\n";
		return 1;
	}
	catch(...) {
		std::cerr << "Exception of unknown type!\n";
	}
	return 0;
}
