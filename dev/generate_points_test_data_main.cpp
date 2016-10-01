#include <boost/format.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/Util/All.h>
#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreAbstract/All.h>

#include <string>
#include <iostream>
#include <vector>

Alembic::AbcGeom::OXform
addXform(Alembic::Abc::OObject parent, std::string name)
{
	Alembic::AbcGeom::OXform xform(parent, name.c_str());

	return xform;
}
Alembic::Abc::OObject
addBody(Alembic::Abc::OObject parent, std::string name)
{
	Alembic::Abc::OObject bodyObject(parent, name.c_str());

	return bodyObject;
}

void animate_points(Alembic::Util::uint32_t            i_num_points,
					Alembic::Util::uint32_t            i_num_time_samples,
					float                              i_velocity_scale,
					Alembic::AbcCoreAbstract::chrono_t i_fps,
					const std::string&                 i_abc_fileName)
{
	Alembic::Abc::OArchive archive(Alembic::Abc::CreateArchiveWithInfo(Alembic::AbcCoreOgawa::WriteArchive(),
			std::string(i_abc_fileName.c_str()),
			std::string("Procedural Insight Pty. Ltd."),
			std::string("Varying Random point count"),
			Alembic::Abc::ErrorHandler::kThrowPolicy));

	Alembic::AbcGeom::OObject iParent( archive, Alembic::AbcGeom::kTop );
	Alembic::AbcGeom::OXform xform = addXform(iParent,"Xform");

	Alembic::AbcCoreAbstract::TimeSampling ts(1/i_fps, 1/i_fps);
	std::cout << boost::format("getNumStoredTimes = %1%") % ts.getNumStoredTimes() << std::endl;
	std::cout << boost::format("BEFORE num_time_samplings = %1%") % iParent.getArchive().getNumTimeSamplings() << std::endl;
	Alembic::Util::uint32_t tsidx = iParent.getArchive().addTimeSampling(ts);
	std::cout << boost::format("AFTER num_time_samplings = %1%") % iParent.getArchive().getNumTimeSamplings() << std::endl;
	// Create our object.
	Alembic::AbcGeom::OPoints partsOut( xform, "constantPointCount", tsidx );

	std::cout << boost::format("i_num_time_samples = %1%") % i_num_time_samples << std::endl;
	Alembic::AbcGeom::OPointsSchema &pSchema = partsOut.getSchema();
	// pSchema.setTimeSampling(tsidx);
	srand48(0);
	// Initialize some known starting position(s)
	std::vector<Alembic::AbcGeom::V3f> last_positions(i_num_points);
	for (size_t pIndex = 0; pIndex < i_num_points; pIndex++)
	{
		last_positions[pIndex] = Alembic::AbcGeom::V3f(drand48() - 0.5,drand48() - 0.5,drand48() - 0.5);
	}
	for (Alembic::Abc::uint32_t sample_index = 0; sample_index < i_num_time_samples; sample_index++)
	{
		std::vector<Alembic::Util::uint64_t> m_ids(i_num_points);
		std::vector<Alembic::AbcGeom::V3f> m_positions(i_num_points);
		std::vector<Alembic::AbcGeom::V3f> m_velocities(i_num_points);
		for (size_t pIndex = 0; pIndex < i_num_points; pIndex++)
		{
			m_ids[pIndex] = pIndex;
			float vel_x = drand48() - 0.5;
			float vel_y = drand48() - 0.5;
			float vel_z = drand48() - 0.5;
			m_positions[pIndex].x = last_positions[pIndex].x + vel_x / i_fps;
			m_positions[pIndex].y = last_positions[pIndex].y + vel_y / i_fps;
			m_positions[pIndex].z = last_positions[pIndex].z + vel_z / i_fps;
			m_velocities[pIndex] = Alembic::AbcGeom::V3f(i_velocity_scale * vel_x,
														 i_velocity_scale * vel_y,
														 i_velocity_scale * vel_z);

			last_positions[pIndex] = m_positions[pIndex];

		}
		Alembic::AbcGeom::V3fArraySample position_data ( m_positions );
		Alembic::AbcGeom::V3fArraySample velocity_data ( m_velocities );
		Alembic::AbcGeom::UInt64ArraySample id_data ( m_ids );
		Alembic::AbcGeom::OPointsSchema::Sample psamp(position_data,id_data,m_velocities);
		pSchema.set( psamp );
	}

	std::cout << boost::format("Number of samples added = %1%") % pSchema.getNumSamples() << std::endl;

}

int main(int argc, char **argv)
{
	if (argc!=3)
	{
		std::cerr << boost::format("Usage : %1% <number-of-points> <output-alembic-file>") % argv[0] << std::endl;
		return 1;
	}
	Alembic::Util::uint32_t num_points = atoi(argv[1]);
	Alembic::Util::uint32_t num_time_samples = 240;
	Alembic::AbcCoreAbstract::chrono_t iFps = 24.0;
	float velocity_scale = 5.0;
	std::string abc_filename(argv[2]);
	animate_points(num_points, num_time_samples, velocity_scale, iFps, abc_filename);
	return 0;
}
