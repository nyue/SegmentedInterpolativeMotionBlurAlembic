#include "ArnoldPolyMeshSchemaHandler.h"

ArnoldPolyMeshSchemaHandler::ArnoldPolyMeshSchemaHandler()
{

}

ArnoldPolyMeshSchemaHandler::~ArnoldPolyMeshSchemaHandler()
{

}

void ArnoldPolyMeshSchemaHandler::process() const
{
	std::cout << "00000" << std::endl;
    Alembic::AbcGeom::IPolyMesh mesh(top,child_name);
    Alembic::AbcGeom::IPolyMeshSchema& schema = mesh.getSchema();

	Alembic::Abc::IV3fArrayProperty velocities_property = schema.getVelocitiesProperty();
	if (velocities_property.valid())
	{
		std::cout << "00100" << std::endl;
		StringContainer       _concatenated_hierachy_path = i_hierachy_path;
		_concatenated_hierachy_path.push_back(child_name);
		flatten_string_array(_concatenated_hierachy_path, "_", unique_object_path);


		Alembic::AbcGeom::TimeSamplingPtr ts_ptr = mesh.getSchema().getTimeSampling();
		Alembic::AbcGeom::TimeSamplingType timeType = ts_ptr->getTimeSamplingType();
		Alembic::AbcGeom::chrono_t tpc = timeType.getTimePerCycle();
		// Alembic::AbcGeom::chrono_t fps = 1.0/tpc;
		// std::cout << boost::format("fps = %1%") % fps << std::endl;
		if ( timeType.isUniform() )
		{
			size_t start_frame = ts_ptr->getStoredTimes()[0] / tpc;
			// std::cout << boost::format("start_frame = %1%") % start_frame << std::endl;
			std::string arnold_filename = (boost::format("%s.%04d.ass") % unique_object_path % i_requested_index).str();
			export_polymesh_as_arnold_ass(mesh,start_frame,i_requested_index,arnold_filename,num_motion_samples,i_relative_shutter_open,i_relative_shutter_close);
		}
	}

}
