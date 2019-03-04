#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

void test_function() {
	SEER_TIME_FUNCTION
}

TEST_CASE("ScopeTimer produces correct json", "[ScopeTimer]") {

	test_helper::reset_seer();

	SECTION("0 event") {
		REQUIRE(seer::buffer.str() == "[]");
	}

	SECTION("1 event") {
		{
			seer::ScopeTimer test("Test");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		test_helper::is_complete_event(json[0], "Test");
	}

	SECTION("2 events") {
		{
			seer::ScopeTimer test("Test");
			seer::ScopeTimer test2("Test2");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 2);
		test_helper::is_complete_event(json[0], "Test2");
		test_helper::is_complete_event(json[1], "Test");
	}

	SECTION("large amount") {
		const auto events_to_create = 50000;
		for (auto i = 0; i < events_to_create; i++)
		{
			seer::ScopeTimer test("Test");

		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == events_to_create);

		for (const auto& event : json)
		{
			test_helper::is_complete_event(event, "Test");
		}
	}

	SECTION("function marco") {
		test_function();
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		test_helper::is_complete_event(json[0], "test_function");
	}
}
