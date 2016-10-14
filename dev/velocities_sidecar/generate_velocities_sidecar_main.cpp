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

#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "VelocitySideCar.h"

namespace po = boost::program_options;

// Get the size of the file by its file descriptor
unsigned long get_size_by_fd(int fd)
{
    struct stat statbuf;
    if(fstat(fd, &statbuf) < 0) exit(-1);
    return statbuf.st_size;
}

void get_md5(const std::string& i_filename, std::string& o_md5)
{
	unsigned char result[MD5_DIGEST_LENGTH];

    int file_descript;
    unsigned long file_size;
    char* file_buffer;

    file_descript = open(i_filename.c_str(), O_RDONLY);
    if(file_descript < 0) exit(-1);

    file_size = get_size_by_fd(file_descript);
    printf("file size:\t%lu\n", file_size);

    file_buffer = (char *)mmap(0, file_size, PROT_READ, MAP_SHARED, file_descript, 0);
    MD5((unsigned char*) file_buffer, file_size, result);
    munmap(file_buffer, file_size);

    o_md5 = reinterpret_cast<char*>(result);
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

		std::string md5;
		get_md5(alembic_file, md5);
		std::cout << boost::format("md5 = '%1%'") % md5 << std::endl;

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
