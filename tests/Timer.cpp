#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

TEST_CASE("ScopeTimer produces correct json", "[ScopeTimer]") {

	test_helper::reset_seer();

	SECTION("1 event") {
		{
			seer::Timer test("Test");
			test.end();
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		test_helper::is_complete_event(json[0], "Test");
	}

	SECTION("2 events") {
		{
			seer::Timer test("Test");
			seer::Timer test2("Test2");
			test2.end();
			test.end();
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 2);
		test_helper::is_complete_event(json[0], "Test2");
		test_helper::is_complete_event(json[1], "Test");
	}
}
