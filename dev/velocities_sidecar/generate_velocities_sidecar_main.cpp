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

#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>

#include "VelocitySideCar.h"

namespace po = boost::program_options;

void get_sha(const std::string& i_filename, std::string& o_sha)
{
	CryptoPP::SHA1 hash;
	CryptoPP::FileSource(i_filename.c_str(),true,
			new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(
					new CryptoPP::StringSink(o_sha), true)));
}

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

		if (vm.count("help") || alembic_file.empty() || velocity_file.empty()) {
			std::cout << desc << "\n";
			return 1;
		}

		std::string sha;
		get_sha(alembic_file, sha);
		std::cout << boost::format("sha = '%1%'") % sha << std::endl;

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
