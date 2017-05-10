#ifndef PIPESPY_HPP
#define PIPESPY_HPP

#include "../../include/Seer.hpp"

#include "../../3rdParty/json/json.hpp"

#include <future>
#include <string>
#include <utility>
#include <map>

class PipeSpy : Seer::BaseSink
{
public:
	PipeSpy()
	{

	};
	~PipeSpy() override {};

	void send(const std::string& data) override
	{		
		auto json = nlohmann::json::parse(data);
		auto match = _matches.find(json["#"]);
		if (match != _matches.end()) {
			match->second.set_value(json);
			_matches.erase(match);
		}
	}

	std::future<nlohmann::json> get_match(std::string message_type) {
		auto promise = std::promise<nlohmann::json>();
		auto future = promise.get_future();
		_matches[message_type] = std::move(promise);
		return future;
	}
private:
	std::map<std::string, std::promise<nlohmann::json>> _matches;
};

#endif