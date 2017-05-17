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
		std::lock_guard<std::mutex> lock(_matches_mutex);
		auto json = nlohmann::json::parse(data);
		auto match = _matches.find(json["#"]);
		if (match != _matches.end()) {
			for (auto& promise : match->second)
			{
				promise.set_value(json);
			}
			_matches.erase(match);
		}
	}

	std::future<nlohmann::json> get_match(std::string message_type) {
		std::lock_guard<std::mutex> lock(_matches_mutex);
		auto promise = std::promise<nlohmann::json>();
		auto future = promise.get_future();
		_matches[message_type].push_back(std::move(promise));
		return future;
	}
private:
	std::map<std::string, std::vector<std::promise<nlohmann::json>>> _matches;
	std::mutex _matches_mutex;
};

#endif