#ifndef DATAPOINT_HPP
#define DATAPOINT_HPP
#include "json\json.hpp"

#include <thread> //thread::id
#include <functional> //std::hash
#include <chrono> //steady_clock::time_point
#include <string>
#include <exception>
#include <stdio.h> //sscanf
#include <sstream>
namespace Seer {
	namespace DataPoint {
		
		struct BaseDataPoint
		{	
			virtual ~BaseDataPoint() {}
			/**
			* \brief: Allow this to be created from json, will throw if json object is incorrect for this TimePoint
			* \return this
			*/
			//virtual BaseDataPoint& operator=(const nlohmann::json& other) // copy assignment
			//{				
			//	return *this;
			//}
			/**
			* \brief: Get the type for this data point
			* \return string of the type
			*/
			virtual const std::string get_type() const
			{
				return "base";
			}
			/**
			* \brief: Get the json data struct for this data point
			* \return json to send over network
			*/
			virtual nlohmann::json get_json() const
			{
				return {};
			}

			/**
			* \brief: Get the json data struct for this data point
			* \return json in string form to send over network
			*/
			virtual std::string get_json_string() const
			{
				return "{}";
			}

			virtual void print_data(std::ostream &out_stream) const = 0;
		};

		inline std::ostream& operator<<(std::ostream& out, const BaseDataPoint& base_data_point)
		{
			base_data_point.print_data(out);
			return out;
		}

		

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
			* \brief: Allow this to be created from json, will throw if json object is incorrect for this TimePoint
			* \return this
			*/
			//virtual TimePoint& operator=(const nlohmann::json& json) override
			//{
			//	if (json["#"].is_string() &&
			//		json["n"].is_string() &&
			//		json["t_id"].is_string() &&
			//		json["s"].is_boolean() &&
			//		json["t"].is_string())
			//	{					
			//		name = json["n"].get<decltype(name)>();
			//		const auto thread_id_string = json["t_id"].get<std::string>();
			//		sscanf(thread_id_string.c_str(), "%zu", &thread_id);
			//		start = json["s"].get<decltype(start)>();
			//		//time_point = json["t"].get<int>();
			//	}
			//	else {
			//		throw std::invalid_argument("must be ");
			//	}

			//	return *this;
			//}

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
					{ "s", start },
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
					{ "s", start },
					{ "t", time_point.time_since_epoch().count() }
				}).dump();
			}

			void print_data(std::ostream &out_stream) const
			{
				out_stream << "{\"#\":\"" << type << "\",\"n\":\"" << name << "\",\"t_id\":" << thread_id << ",\"s\":" << start << ",\"t\":" << time_point.time_since_epoch().count() << "}";
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