#ifndef BASELISTENER_HPP
#define BASELISTENER_HPP

#include <string>
#include <functional>
#include <utility>

namespace Seer {
	namespace Listener {
		class BaseListener
		{
		public:
			BaseListener(std::string name, std::function<void()> callback) :
				_name(name),
				_callback(std::move(_callback))
			{
			}
			virtual ~BaseListener();
		private:
			std::string _name;
			std::function<void()> _callback;
		};
	}
}
#endif