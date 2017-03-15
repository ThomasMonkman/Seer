#ifndef BASELISTENER_HPP
#define BASELISTENER_HPP

#include "Network.hpp"

#include <string>
#include <functional>
#include <utility>

namespace Seer {
	namespace Listener {
		class BaseListener
		{
			friend class Network::Network;
		public:
			BaseListener(std::string name, std::function<void()> callback) :
				_name(name),
				_callback(std::move(_callback))
			{
			}
			virtual ~BaseListener();
			virtual const std::string get_type() const
			{
				return "base";
			}
		protected:			
			std::string _name;
			std::function<void()> _callback;
		};
	}
}
#endif