#ifndef TIMEPOINT_HPP
#define TIMEPOINT_HPP
#include "DataPoint.hpp"
#include "json/json.hpp"
namespace Seer {
	namespace DataPoint {
		struct TimePoint : public BaseDataPoint
		{
			/**
			* \brief: Create a datapoint specialised around timers
			* \param name of the timepoint, this will be decide how it is sorted and grouped,
			* aka all timepoints with name "draw" will put in to the draw graph
			* \param thread_id is thread this time point should be attached to
			* again this is used for sorting and grouping the timepoint correctly
			* \param start where or not this is the start of the time point
			* \param time_point to store
			*/
			TimePoint(std::string name,
				std::thread::id thread_id,
				bool start,
				std::chrono::steady_clock::time_point time_point) :
				name(name),
				thread_id(std::hash<std::thread::id>()(thread_id)),
				start(start),
				time_point(time_point)
			{}

			~TimePoint() override
			{
			}

			const std::string type = { "tp" };
			std::string name;
			std::size_t thread_id;
			bool start = { true };
			std::chrono::steady_clock::time_point time_point;

			/**
			* \brief: Get the type for this data point
			* \return string of the type
			*/
			const std::string get_type() const override
			{
				return type;
			}

			/**
			* \brief: Get the json data struct for this data point
			* \return json to send over network
			*/
			nlohmann::json get_json() const override
			{
				return {
					{ "#", type },
					{ "n", name },
					{ "t_id", thread_id },
					{ "p", start },
					{ "t", time_point.time_since_epoch().count() }
				};
			}

			/**
			* \brief: Get the json data struct for this data point
			* \return json in string form to send over network
			*/
			std::string get_json_string() const override
			{
				/*std::stringstream json;
				json << "{\"#\":\"" << type << "\",\"n\":\"" << name << "\",\"t_id\":" << thread_id << ",\"s\":" << start << ",\"t\":" << time_point.time_since_epoch().count() << "}";
				return json.str();*/
				return nlohmann::json({
					{ "#", type },
					{ "n", name },
					{ "t_id", thread_id },
					{ "p", start },
					{ "t", time_point.time_since_epoch().count() }
				}).dump();
			}


			/**
			* \brief: output this timepoint as json to the supplied ostream
			* \param out_stream to write to
			*/
			void print_data(std::ostream &out_stream) const override
			{
				out_stream << "{\"#\":\"" << type << "\",\"n\":\"" << name << "\",\"t_id\":" << thread_id << ",\"p\":" << start << ",\"t\":" << time_point.time_since_epoch().count() << "}";
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