#include "AbstractPointsSchemaHandler.h"

AbstractPointsSchemaHandler::AbstractPointsSchemaHandler()
{

}

AbstractPointsSchemaHandler::~AbstractPointsSchemaHandler()
{

}

void AbstractPointsSchemaHandler::ProcessPoints(const Alembic::Abc::IObject& top,
												const std::string&           child_name,
												const StringContainer&       i_hierachy_path,
												size_t 					     i_requested_index,
												float          			     i_relative_shutter_open,
												float          			     i_relative_shutter_close,
												Alembic::Abc::uint8_t        num_motion_samples) const
{
	std::cout << " of type Points";
    Alembic::AbcGeom::IPoints points(top,child_name);
    Alembic::AbcGeom::IPointsSchema& schema = points.getSchema();

	Alembic::Abc::IV3fArrayProperty velocities_property = schema.getVelocitiesProperty();
	Alembic::Abc::IUInt64ArrayProperty ids_property = schema.getIdsProperty();
	if (velocities_property.valid() && ids_property.valid())
	{
		std::string unique_object_path;
		StringContainer       _concatenated_hierachy_path = i_hierachy_path;
		_concatenated_hierachy_path.push_back(child_name);
		flatten_string_array(_concatenated_hierachy_path, "_", unique_object_path);
		std::cout << boost::format(" of type Points, unique_object_path : '%1%'") % unique_object_path << std::endl;

		Alembic::AbcGeom::TimeSamplingPtr ts_ptr = points.getSchema().getTimeSampling();
		Alembic::AbcGeom::TimeSamplingType timeType = ts_ptr->getTimeSamplingType();
		Alembic::AbcGeom::chrono_t tpc = timeType.getTimePerCycle();
		// Alembic::AbcGeom::chrono_t fps = 1.0/tpc;
		// std::cout << boost::format("fps = %1%") % fps << std::endl;
		if ( timeType.isUniform() )
		{
    		std::cout << " timeType.isUniform() == true" << std::endl;
			size_t start_frame = ts_ptr->getStoredTimes()[0] / tpc;
			// std::cout << boost::format("start_frame = %1%") % start_frame << std::endl;
			std::string renderman_filename = (boost::format("%s.%04d.rib") % unique_object_path % i_requested_index).str();
    		std::cout << boost::format(" renderman_filename : '%1%'") % renderman_filename << std::endl;
			EmitPoints(points,start_frame,i_requested_index,renderman_filename,num_motion_samples,i_relative_shutter_open,i_relative_shutter_close);
		}
		else if (timeType.isCyclic())
		{
    		std::cout << " timeType.isCyclic() == true" << std::endl;
		}
		else if (timeType.isAcyclic())
		{
    		std::cout << " timeType.isAcyclic() == true" << std::endl;
		}
	}
}
