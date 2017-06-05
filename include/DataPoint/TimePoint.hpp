#ifndef TIMEPOINT_HPP
#define TIMEPOINT_HPP
#include "DataPoint.hpp"
#include "json/json.hpp"
namespace Seer {
	namespace DataPoint {
		/** 
		* \class TimePoint
		* \brief Create a datapoint specialised around timers.
		*/
		struct TimePoint : public BaseDataPoint
		{
			/**
			* \brief Create a datapoint specialised around timers.
			* \param name of the timepoint, this will be decide how it is sorted and grouped,
			* aka all timepoints with name "draw" will put in to the draw graph.
			* \param thread_id is thread this time point should be attached to
			* again this is used for sorting and grouping the timepoint correctly.
			* \param position of this timepoint in reference to other timepoints it should be groups with, for example 0 for start of a time block, 0 for the end of it.
			* \param time_point to store.
			*/
			TimePoint(std::string name,
				std::thread::id thread_id,
				std::size_t position,
				std::chrono::steady_clock::time_point time_point) :
				name(name),
				thread_id(std::hash<std::thread::id>()(thread_id)),
				position(position),
				time_point(time_point)
			{}

			~TimePoint() override
			{
			}

			const std::string type = { "tp" };
			const std::string name;
			const std::size_t thread_id;
			const std::size_t position = { true };
			const std::chrono::steady_clock::time_point time_point;

			/**
			* \brief: Get the type for this data point.
			* \return string of the type.
			*/
			const std::string get_type() const override
			{
				return type;
			}

			/**
			* \brief: Get the json data struct for this data point.
			* \return json to send over network.
			*/
			nlohmann::json get_json() const override
			{
				return {
					{ "#", type },
					{ "n", name },
					{ "t_id", thread_id },
					{ "p", position },
					{ "t", time_point.time_since_epoch().count() }
				};
			}

			/**
			* \brief: output this TimePoint as json to the supplied ostream.
			* \param out_stream to write to.
			*/
			void print_data(std::ostream &out_stream) const override
			{
				out_stream << "{\"#\":\"" << type << "\",\"n\":\"" << name << "\",\"t_id\":" << thread_id << ",\"p\":" << position << ",\"t\":" << time_point.time_since_epoch().count() << "}";
			};
		};

		inline std::ostream& operator<<(std::ostream& out, const TimePoint& time_point)
		{
			time_point.print_data(out);
			return out;
		}
	}
}
#endif