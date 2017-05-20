#ifndef PIPESPY_HPP
#define PIPESPY_HPP

//#include "../../include/Seer.hpp"
#include "Seer.hpp"

#include "../../3rdParty/json/json.hpp"

#include <future>
#include <string>
#include <utility>
#include <map>
#include <mutex>


class PipeSpy : public Seer::BaseSink
{
public:
	PipeSpy()
	{

	};
	~PipeSpy() override {};

	void send(const std::string& data) override
	{
		std::lock_guard<std::mutex> lock(_callbacks_mutex);
		auto json = nlohmann::json::parse(data);
		for (const auto& data_point : json)
		{
			for (auto& promise : _callbacks)
			{
				if (promise.predicate(data_point)) {
					promise.promise.set_value(data_point);
				}
			}
		}
	}

	std::future<nlohmann::json> get_match(std::function<bool(const nlohmann::json&)> predicate) {
		std::lock_guard<std::mutex> lock(_callbacks_mutex);
		ConditionalPromise<nlohmann::json> conditional_promise(predicate);
		auto future = conditional_promise.promise.get_future();
		_callbacks.push_back(std::move(conditional_promise));
		return future;
	}
private:
	template<typename T>
	struct ConditionalPromise
	{
		ConditionalPromise(std::function<bool(const T&)> predicate)
			: predicate(predicate), promise() {}
		std::function<bool(T)> predicate;
		std::promise<T> promise;
	};
	std::vector<ConditionalPromise<nlohmann::json>> _callbacks;
	std::mutex _callbacks_mutex;
};

#endif