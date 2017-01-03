#ifndef DATAPOINT_HPP
#define DATAPOINT_HPP

#include <thread> //thread::id
#include <functional> //std::hash
#include <chrono> //steady_clock::time_point
#include <string>
#include <3rdParty\json\json.hpp>
namespace Seer {
	namespace DataPoint {
		
		struct BaseTimePoint
		{			
			virtual nlohmann::json get_json() const
			{
				return {};
			}
		};

		struct TimePoint : BaseTimePoint
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
				thread_id(thread_id),
				start(start),
				time_point(time_point)
			{}

			const std::string type = { "tp" };
			std::string name;
			std::thread::id thread_id;
			bool start = { true };
			std::chrono::steady_clock::time_point time_point;

			/**
			* \brief: Create a datapoint specialised around timers
			* \return json to send over network
			*/
			nlohmann::json get_json() const override
			{
				return { {
					{ "#", type},
					{ "n", name},
					{ "t_id",  std::hash<std::thread::id>()(thread_id) },
					{ "s", start },
					{ "t", time_point.time_since_epoch().count() },
				} };
			}
		};
	}
}
#endif