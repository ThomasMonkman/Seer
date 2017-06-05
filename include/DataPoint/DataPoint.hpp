#ifndef DATAPOINT_HPP
#define DATAPOINT_HPP
#include "json/json.hpp"

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
			* \brief: Get the type for this data point.
			* \return string of the type.
			*/
			virtual const std::string get_type() const
			{
				return "base";
			}
			/**
			* \brief: Get the json data struct for this data point.
			* \return json to send over network.
			*/
			virtual nlohmann::json get_json() const
			{
				return {};
			}

			/**
			* \brief: output this DataPoint as json to the supplied ostream.
			* \param out_stream to write to.
			*/
			virtual void print_data(std::ostream &out_stream) const
			{
				out_stream << "{}";
			}
		};

		inline std::ostream& operator<<(std::ostream& out, const BaseDataPoint& base_data_point)
		{
			base_data_point.print_data(out);
			return out;
		}
	}
}
#endif