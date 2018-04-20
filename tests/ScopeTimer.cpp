#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

void isCompleteEvent(const nlohmann::json& event, const std::string& name) {
	REQUIRE(event["name"] == name);
	REQUIRE(event["ph"] == "X");
	REQUIRE(event["ts"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["dur"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["pid"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["tid"].type() == nlohmann::json::value_t::number_unsigned);
}

TEST_CASE("ScopeTimer produces correct json", "[ScopeTimer]") {

	seer::buffer.clear();

	SECTION("0 event") {
		REQUIRE(seer::buffer.str() == "[]");
	}

	SECTION("1 event") {
		{
			seer::ScopeTimer test("Test");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		isCompleteEvent(json[0], "Test");
	}

	SECTION("2 events") {
		{
			seer::ScopeTimer test("Test");
			seer::ScopeTimer test2("Test2");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 2);
		isCompleteEvent(json[0], "Test2");
		isCompleteEvent(json[1], "Test");
	}
}
