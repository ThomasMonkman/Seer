#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

void is_mark_event(const nlohmann::json& event, const std::string& name) {
	REQUIRE(event["name"] == name);
	REQUIRE(event["ph"] == "R");
	REQUIRE(event["ts"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["pid"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["tid"].type() == nlohmann::json::value_t::number_unsigned);
}

TEST_CASE("mark produces correct json", "[seer::mark]") {

	seer::buffer.clear();

	SECTION("0 event") {
		REQUIRE(seer::buffer.str() == "[]");
	}

	SECTION("1 event") {
		{
			seer::mark("Test");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		is_mark_event(json[0], "Test");
	}

	SECTION("2 events") {
		{
			seer::mark("Test");
			seer::mark("Test2");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 2);
		is_mark_event(json[0], "Test");
		is_mark_event(json[1], "Test2");
	}
}
