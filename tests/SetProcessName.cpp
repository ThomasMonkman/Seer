#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

void is_process_name_event(const nlohmann::json& event) {
	REQUIRE(event["name"] == "process_name");
	REQUIRE(event["ph"] == "M");
	REQUIRE(event["ts"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["pid"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["tid"].type() == nlohmann::json::value_t::string);
	REQUIRE(event["args"].type() == nlohmann::json::value_t::object);
	REQUIRE(event["args"].find("name") != event["args"].end());
	REQUIRE(event["args"]["name"].type() == nlohmann::json::value_t::string);
}


TEST_CASE("Sets process name", "[seer::set_process_name]") {

	seer::buffer.clear();

	SECTION("sets current process name") {
		seer::set_process_name("Test");

		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		is_process_name_event(json[0]);
		REQUIRE(json[0]["args"]["name"].get<std::string>() == std::string("Test"));
	}
}