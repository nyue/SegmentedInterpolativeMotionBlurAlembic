#include "AbstractPolyMeshSchemaHandler.h"

AbstractPolyMeshSchemaHandler::AbstractPolyMeshSchemaHandler()
{

}

AbstractPolyMeshSchemaHandler::~AbstractPolyMeshSchemaHandler()
{

}

void AbstractPolyMeshSchemaHandler::ProcessPolyMesh(const Alembic::Abc::IObject& top,
													const std::string&           child_name,
													const StringContainer&       i_hierachy_path,
													size_t 					     i_requested_index,
													float          			     i_relative_shutter_open,
													float          			     i_relative_shutter_close,
													Alembic::Abc::uint8_t		 num_motion_samples) const
{
	Alembic::AbcGeom::IPolyMesh mesh(top,child_name);
	Alembic::AbcGeom::IPolyMeshSchema& schema = mesh.getSchema();
	std::string unique_object_path;
	Alembic::Abc::IV3fArrayProperty velocities_property = schema.getVelocitiesProperty();
	if (velocities_property.valid())
	{
		StringContainer       _concatenated_hierachy_path = i_hierachy_path;
		_concatenated_hierachy_path.push_back(child_name);
		flatten_string_array(_concatenated_hierachy_path, "_", unique_object_path);


		Alembic::AbcGeom::TimeSamplingPtr ts_ptr = mesh.getSchema().getTimeSampling();
		Alembic::AbcGeom::TimeSamplingType timeType = ts_ptr->getTimeSamplingType();
		Alembic::AbcGeom::chrono_t tpc = timeType.getTimePerCycle();
		if ( timeType.isUniform() )
		{
			size_t start_frame = ts_ptr->getStoredTimes()[0] / tpc;
			std::string renderman_ass_filename = (boost::format("%s.%04d.rib") % unique_object_path % i_requested_index).str();
			EmitPolyMesh(mesh,start_frame,i_requested_index,renderman_ass_filename,num_motion_samples,i_relative_shutter_open,i_relative_shutter_close);
		}
	}
}
