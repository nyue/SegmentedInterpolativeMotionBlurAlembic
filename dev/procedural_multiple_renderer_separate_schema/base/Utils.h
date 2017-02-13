#pragma once

#include "Types.h"
#include <fstream>
#include <boost/format.hpp>
#include <Alembic/AbcGeom/All.h>
#include <logging.h>

bool build_even_motion_relative_time_samples(float                 i_relative_shutter_open,
											 float                 i_relative_shutter_close,
											 Alembic::Abc::uint8_t i_motion_sample_count,
											 FloatContainer&       o_earlier_sampling_time_vector,
											 FloatContainer&       o_later_sampling_time_vector);
bool build_single_even_motion_relative_time_samples(float                 i_relative_shutter_open,
													float                 i_relative_shutter_close,
													Alembic::Abc::uint8_t i_motion_sample_count,
													FloatContainer&       o_sampling_time_vector);

void build_interim_points(const Alembic::AbcGeom::IPointsSchema::Sample* i_sample,
						  AlembicPointsDataIndexedMap&                   o_interim_points);

template <typename T> void interpolate(const Imath::Vec3<T>& P1,
									   const Imath::Vec3<T>& T1,
									   const Imath::Vec3<T>& P2,
									   const Imath::Vec3<T>& T2,
									   T                     s,
									   Imath::Vec3<T>&       P)
{
	T h1 =  2.0*std::pow(s,3.0) - 3.0*std::pow(s,2.0) + 1.0; // calculate basis function 1
	T h2 = -2.0*std::pow(s,3.0) + 3.0*std::pow(s,2.0);       // calculate basis function 2
	T h3 =      std::pow(s,3.0) - 2.0*std::pow(s,2.0) + s;   // calculate basis function 3
	T h4 =      std::pow(s,3.0) -     std::pow(s,2.0);       // calculate basis function 4


	P = h1*P1 + // multiply and sum all functions
		h2*P2 + // together to build the interpolated
		h3*T1 + // point along the curve.
		h4*T2;
}

template <typename P, typename I, typename W>
bool build_points_for_renderer_from_interim_points(const AlembicPointsDataIndexedMap* i_previous_interim_points,
												   const AlembicPointsDataIndexedMap* i_current_interim_points,
												   const AlembicPointsDataIndexedMap* i_next_interim_points,
												   float							  i_relative_shutter_open,
												   float							  i_relative_shutter_close,
												   Alembic::Abc::uint8_t              i_motion_sample_count,
												   P&								  o_motion_sample_point_positions,
												   I&								  o_ids,
												   W&								  o_widths)
{

	FloatContainer earlier_sampling_time_vector;
	FloatContainer later_sampling_time_vector;


	if (build_even_motion_relative_time_samples(i_relative_shutter_open,
												i_relative_shutter_close,
												i_motion_sample_count,
												earlier_sampling_time_vector,
												later_sampling_time_vector))
	{

		o_motion_sample_point_positions.resize(boost::extents[i_motion_sample_count][i_current_interim_points->size()]);
		V3fSamplingArray2D::index point_index = 0;
		for(AlembicPointsDataIndexedMap::const_iterator iter = i_current_interim_points->begin(); iter != i_current_interim_points->end(); iter++,point_index++)
		{
			uint64_t search_id = iter->first;
			o_ids.push_back(search_id);
			o_widths.push_back(0.1f);
			bool previous_point_exists = false;
			bool next_point_exists = false;
			// Generate samples for this particular point
			typedef std::vector<Imath::V3f> V3fContainer;

			V3fContainer interpolated_P(i_motion_sample_count);
			AlembicPointsDataIndexedMap::const_iterator find_previous_point_result;
			AlembicPointsDataIndexedMap::const_iterator find_next_point_result;
			if (i_previous_interim_points)
			{
				find_previous_point_result = i_previous_interim_points->find(search_id);
				if (find_previous_point_result != i_previous_interim_points->end())
				{
					previous_point_exists = true;
				}
			}

			if (i_next_interim_points)
			{
				find_next_point_result = i_next_interim_points->find(search_id);
				if (find_next_point_result != i_next_interim_points->end())
				{
					next_point_exists = true;
				}
			}

			// The are 3 possibilities - start, in-between and end
			if (!previous_point_exists && next_point_exists)
			{
				// START frame
				DLOG(INFO) << "START frame : points processing" << std::endl;
			}
			else if (previous_point_exists && !next_point_exists)
			{
				// END frame
				DLOG(INFO) << "END frame : points processing" << std::endl;
			}
			else if (previous_point_exists && next_point_exists)
			{
				// INBETWEEN frame
				DLOG(INFO) << "INBETWEEN frame : points processing" << std::endl;
				size_t interpolated_P_index = 0;
				size_t num_earlier_sampling_time = earlier_sampling_time_vector.size();
				size_t num_later_sampling_time = later_sampling_time_vector.size();

				Imath::V3f P1;
				Imath::V3f T1;
				Imath::V3f P2;
				Imath::V3f T2;

				// Previous
				P1 = find_previous_point_result->second._position;
				T1 = find_previous_point_result->second._velocity;
				P2 = iter->second._position;
				T2 = iter->second._velocity;
				for (size_t earlier_sample_index=0;earlier_sample_index<num_earlier_sampling_time;++earlier_sample_index)
				{
					float s = 1+earlier_sampling_time_vector[earlier_sample_index];
					interpolate<float>(P1,T1,P2,T2,s,interpolated_P[interpolated_P_index]);
					interpolated_P_index++;
				}

				// Next
				P1 = iter->second._position;
				T1 = iter->second._velocity;
				P2 = find_next_point_result->second._position;
				T2 = find_next_point_result->second._velocity;
				for (size_t next_sample_index=0;next_sample_index<num_later_sampling_time;++next_sample_index)
				{
					float s = later_sampling_time_vector[next_sample_index];
					interpolate<float>(P1,T1,P2,T2,s,interpolated_P[interpolated_P_index]);
					interpolated_P_index++;
				}

				assert ( interpolated_P_index == i_motion_sample_count );
			}
			for (V3fSamplingArray2D::index motion_index = 0;motion_index<i_motion_sample_count;++motion_index)
			{
				o_motion_sample_point_positions[motion_index][point_index] = interpolated_P[motion_index];
			}
		}
		return true;
	}
	return false;
}

template <typename P, typename I, typename W>
void write_renderer_points_data_to_csv_file(const P&			      i_motion_sample_point_positions,
											const I&			      i_ids,
											const W&			      i_widths,
											V3fSamplingArray2D::index i_sample_index,
											const std::string&        i_csv_filename)
{
	std::ofstream csv_file;
	csv_file.open (i_csv_filename.c_str());

    size_t per_sample_point_count = i_motion_sample_point_positions[i_sample_index].size();
    size_t width_count = i_widths.size();

    assert ( per_sample_point_count == width_count );

    csv_file << "x,y,z,width" << std::endl;

    for (size_t index=0;index<per_sample_point_count;index++)
    {
    	float x = i_motion_sample_point_positions[i_sample_index][index].x;
    	float y = i_motion_sample_point_positions[i_sample_index][index].y;
    	float z = i_motion_sample_point_positions[i_sample_index][index].z;
    	float w = i_widths[index];
        csv_file << boost::format("%1%, %2%, %3%, %4%") % x % y % z % w << std::endl;
    }

    csv_file.close();
}

template <typename P, typename I, typename W>
void write_renderer_points_data_to_csv_sequence(const P&		 i_motion_sample_point_positions,
											  const I&			 i_ids,
											  const W&			 i_widths,
											  const std::string& i_base_filename)
{
	V3fSamplingArray2D::index num_samples = i_motion_sample_point_positions.size();
	V3fSamplingArray2D::index num_elements_per_sample = i_motion_sample_point_positions.shape()[1];
	size_t num_radii = i_widths.size();
	size_t num_ids = i_ids.size();
	if ( num_radii != num_elements_per_sample )
	{
		LOG(ERROR) << boost::format("num_radii = %1% != num_elements_per_sample = %2%") % num_radii % num_elements_per_sample << std::endl;
		return;
	}
	if ( num_ids != num_elements_per_sample )
	{
		LOG(ERROR) << boost::format("num_ids = %1% != num_elements_per_sample = %2%") % num_ids % num_elements_per_sample << std::endl;
		return;
	}
	// std::cout << boost::format("num_elements_per_sample = %1%") % num_elements_per_sample << std::endl;
	for (V3fSamplingArray2D::index sample_index=0;sample_index<num_samples;++sample_index)
	{
		std::string numbered_output_filename = (boost::format(i_base_filename) % (sample_index+1)).str();
		// std::cout << boost::format("numbered_output_filename = '%1%'") % numbered_output_filename << std::endl;
		write_renderer_points_data_to_csv_file<P,I,W>(i_motion_sample_point_positions,
													  i_ids,
													  i_widths,
													  sample_index,
													  numbered_output_filename);
	}
	// std::cout << boost::format("_vlist_data_array num_elements = %1%") % i_arnold_mesh_data._vlist_data_array.size() << std::endl;
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
