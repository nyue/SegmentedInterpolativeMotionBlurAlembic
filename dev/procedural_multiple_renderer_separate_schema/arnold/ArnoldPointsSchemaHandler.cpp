#include "ArnoldPointsSchemaHandler.h"
#include "ArnoldUtils.h"
#include "Utils.h"

ArnoldPointsSchemaHandler::ArnoldPointsSchemaHandler()
{

}

ArnoldPointsSchemaHandler::~ArnoldPointsSchemaHandler()
{

}

void ArnoldPointsSchemaHandler::EmitPoints(Alembic::AbcGeom::IPoints& points,
										   Alembic::Abc::index_t      i_start_frame_number,
										   Alembic::Abc::index_t      i_requested_frame_number,
										   const std::string&         i_arnold_filename,
										   Alembic::Abc::uint8_t      i_motion_samples,
										   float          			  i_relative_shutter_open,
										   float          			  i_relative_shutter_close) const
{

    size_t num_samples = points.getSchema().getNumSamples();
    size_t last_sample_index = num_samples - 1;
    Alembic::Abc::int64_t requested_index = i_requested_frame_number - i_start_frame_number;
    std::cout << boost::format("num_samples = %1% i_requested_frame_number = %2% requested_index = %3% last_sample_index = %4%") % num_samples % i_requested_frame_number % requested_index % last_sample_index << std::endl;
    if (requested_index < 0 || requested_index > (num_samples-1))
    	return;

	Alembic::Abc::ISampleSelector current_sample_selector(requested_index);
    Alembic::AbcGeom::IPointsSchema::Sample current_sample;
    points.getSchema().get( current_sample, current_sample_selector );
    AlembicPointsDataIndexedMap current_interim_points;

    build_interim_points(&current_sample,current_interim_points);

    if (requested_index == 0)
    {
    	// First frame
    	Alembic::Abc::ISampleSelector next_sample_selector(requested_index+1);

        Alembic::AbcGeom::IPointsSchema::Sample next_sample;
    	points.getSchema().get( next_sample, next_sample_selector );

        AlembicPointsDataIndexedMap next_interim_points;
        build_interim_points(&next_sample,next_interim_points);

    	ArnoldPointsData arnold_points_data;
    	build_points_for_renderer_from_interim_points<V3fSamplingArray2D,AtUInt64Container,FloatContainer>(0,
    			    													&current_interim_points,
    																	&next_interim_points,
    																	i_relative_shutter_open,
    																	i_relative_shutter_close,
    																	i_motion_samples,
																		arnold_points_data._points_data_array,
																		arnold_points_data._ids_data,
																		arnold_points_data._radius_data);

    	write_arnold_points_data_to_file(arnold_points_data,
    									 i_arnold_filename,
										 i_relative_shutter_open,
										 i_relative_shutter_close);

    }
    else if (requested_index == last_sample_index)
    {
    	// Last frame
    	Alembic::Abc::ISampleSelector previous_sample_selector(requested_index-1);

        Alembic::AbcGeom::IPointsSchema::Sample previous_sample;
    	points.getSchema().get( previous_sample, previous_sample_selector );

        AlembicPointsDataIndexedMap previous_interim_points;
        build_interim_points(&previous_sample,previous_interim_points);

    	ArnoldPointsData arnold_points_data;
    	build_points_for_renderer_from_interim_points<V3fSamplingArray2D,AtUInt64Container,FloatContainer>(&previous_interim_points,
    			    													&current_interim_points,
    																	0,
    																	i_relative_shutter_open,
    																	i_relative_shutter_close,
    																	i_motion_samples,
																		arnold_points_data._points_data_array,
																		arnold_points_data._ids_data,
																		arnold_points_data._radius_data);

    	write_arnold_points_data_to_file(arnold_points_data,
    									 i_arnold_filename,
										 i_relative_shutter_open,
										 i_relative_shutter_close);

    }
    else
    {
    	std::cout << "NICHOLAS XXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;
    	Alembic::Abc::ISampleSelector previous_sample_selector(requested_index-1);

        Alembic::AbcGeom::IPointsSchema::Sample previous_sample;
    	points.getSchema().get( previous_sample, previous_sample_selector );

        AlembicPointsDataIndexedMap previous_interim_points;
        build_interim_points(&previous_sample,previous_interim_points);

        // ==============

    	Alembic::Abc::ISampleSelector next_sample_selector(requested_index+1);

        Alembic::AbcGeom::IPointsSchema::Sample next_sample;
    	points.getSchema().get( next_sample, next_sample_selector );

        AlembicPointsDataIndexedMap next_interim_points;
        build_interim_points(&next_sample,next_interim_points);

        // ==================
    	ArnoldPointsData arnold_points_data;
    	build_points_for_renderer_from_interim_points<V3fSamplingArray2D,AtUInt64Container,FloatContainer>(&previous_interim_points,
    			    													&current_interim_points,
																		&next_interim_points,
    																	i_relative_shutter_open,
    																	i_relative_shutter_close,
    																	i_motion_samples,
																		arnold_points_data._points_data_array,
																		arnold_points_data._ids_data,
																		arnold_points_data._radius_data);

    	// write_arnold_points_data_to_csv_sequence(arnold_points_data,"points_per_sample.%04d.csv");
    	write_arnold_points_data_to_file(arnold_points_data,
    									 i_arnold_filename,
										 i_relative_shutter_open,
										 i_relative_shutter_close);

    }


}
