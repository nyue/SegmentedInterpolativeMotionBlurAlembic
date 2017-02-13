#include "RendermanPointsSchemaHandler.h"
#include "RendermanUtils.h"
#include "Utils.h"
#include <logging.h>

RendermanPointsSchemaHandler::RendermanPointsSchemaHandler()
{

}

RendermanPointsSchemaHandler::~RendermanPointsSchemaHandler()
{

}

void RendermanPointsSchemaHandler::EmitPoints(Alembic::AbcGeom::IPoints& points,
											  Alembic::Abc::index_t      i_start_frame_number,
											  Alembic::Abc::index_t      i_requested_frame_number,
											  Alembic::Abc::uint8_t 	 i_motion_samples,
											  float          			 i_relative_shutter_open,
											  float          			 i_relative_shutter_close,
											  const char*                i_renderer_filename) const
{

    size_t num_samples = points.getSchema().getNumSamples();
    size_t last_sample_index = num_samples - 1;
    Alembic::Abc::int64_t requested_index = i_requested_frame_number - i_start_frame_number;
    DLOG(INFO) << boost::format("num_samples = %1% i_requested_frame_number = %2% requested_index = %3% last_sample_index = %4%") % num_samples % i_requested_frame_number % requested_index % last_sample_index << std::endl;
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

    	RendermanPointsData renderman_points_data;
    	build_points_for_renderer_from_interim_points<V3fSamplingArray2D,RtIntContainer,RtFloatContainer>(0,
    			    													&current_interim_points,
    																	&next_interim_points,
    																	i_relative_shutter_open,
    																	i_relative_shutter_close,
    																	i_motion_samples,
																		renderman_points_data._P_data_array,
																		renderman_points_data._ids_data,
																		renderman_points_data._widths_data);
    	emit_renderman_points_data(renderman_points_data,
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

    	RendermanPointsData renderman_points_data;

    	build_points_for_renderer_from_interim_points<V3fSamplingArray2D,RtIntContainer,RtFloatContainer>(&previous_interim_points,
    			    													&current_interim_points,
    																	0,
    																	i_relative_shutter_open,
    																	i_relative_shutter_close,
    																	i_motion_samples,
																		renderman_points_data._P_data_array,
																		renderman_points_data._ids_data,
																		renderman_points_data._widths_data);
    	emit_renderman_points_data(renderman_points_data,
								   i_relative_shutter_open,
								   i_relative_shutter_close);

    }
    else
    {
    	DLOG(INFO) << "NICHOLAS XXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;
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
    	RendermanPointsData renderman_points_data;

    	build_points_for_renderer_from_interim_points<V3fSamplingArray2D,RtIntContainer,RtFloatContainer>(&previous_interim_points,
    			    													&current_interim_points,
																		&next_interim_points,
    																	i_relative_shutter_open,
    																	i_relative_shutter_close,
    																	i_motion_samples,
																		renderman_points_data._P_data_array,
																		renderman_points_data._ids_data,
																		renderman_points_data._widths_data);
    	write_renderer_points_data_to_csv_sequence<V3fSamplingArray2D,RtIntContainer,RtFloatContainer>(renderman_points_data._P_data_array,
																									   renderman_points_data._ids_data,
																									   renderman_points_data._widths_data,
																									   "points_per_sample.%04d.csv");
    	emit_renderman_points_data(renderman_points_data,
    							   i_relative_shutter_open,
								   i_relative_shutter_close);

    }



}

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
