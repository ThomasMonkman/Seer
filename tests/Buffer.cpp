#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>
#include <exception>

TEST_CASE("buffer works", "[seer::buffer]") {
	
	test_helper::reset_seer();


	SECTION("start empty") {
		REQUIRE(seer::buffer.usage().usage_in_bytes == 0);
		REQUIRE(seer::buffer.usage().percent_used == 0.0);
	}
	
	SECTION("clears") {
		{
			seer::ScopeTimer test("Test");
		}
		REQUIRE(seer::buffer.str() != "[]");
		seer::buffer.clear();
		REQUIRE(seer::buffer.str() == "[]");
	}

	SECTION("works even when empty") {
		REQUIRE(seer::buffer.str() == "[]");
		seer::buffer.clear();
		REQUIRE(seer::buffer.str() == "[]");
	}

	SECTION("increases in size when used") {
		{
			seer::ScopeTimer test("Test");
		}
		REQUIRE(seer::buffer.usage().usage_in_bytes != 0);
	}

	SECTION("writes to string") {
		{
			seer::ScopeTimer test("Test");
		}
		REQUIRE(seer::buffer.str().size() > 0);
		std::stringstream ss;
		ss << seer::buffer;
		REQUIRE(ss.str().size() > 0);
	}

	SECTION("can resize") {
		seer::buffer.resize(2 * sizeof(seer::internal::DataPoint));
		const auto usage = seer::buffer.usage();
		REQUIRE(seer::buffer.usage().usage_in_bytes == 0);
		REQUIRE(seer::buffer.usage().total_in_bytes == (2 * sizeof(seer::internal::DataPoint)));
		REQUIRE(seer::buffer.usage().percent_used == 0.0);
	}

	SECTION("BufferOverflowBehaviour::reset") {
		seer::buffer_overflow_behaviour = seer::BufferOverflowBehaviour::reset;
		seer::buffer.resize(2 * sizeof(seer::internal::DataPoint)); // not large enough to store 2 timers plus text
		{
			seer::ScopeTimer("Test");
			seer::ScopeTimer("Test");
		}

		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		const auto usage = seer::buffer.usage();
		REQUIRE(seer::buffer.usage().total_in_bytes == (2 * sizeof(seer::internal::DataPoint)));
	}

	SECTION("BufferOverflowBehaviour::expand") {
		seer::buffer_overflow_behaviour = seer::BufferOverflowBehaviour::expand;
		seer::buffer.resize(2 * sizeof(seer::internal::DataPoint)); // not large enough to store 2 timers plus text
		{
			seer::ScopeTimer("Test");
			seer::ScopeTimer("Test");
		}

		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 2);
	}

	SECTION("BufferOverflowBehaviour::discard") {
		seer::buffer_overflow_behaviour = seer::BufferOverflowBehaviour::discard;
		seer::buffer.resize(2 * sizeof(seer::internal::DataPoint)); // not large enough to store 2 timers plus text
		{
			seer::ScopeTimer("Test");
			seer::ScopeTimer("");
			seer::ScopeTimer("very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string_very_big_string");
		}

		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		const auto usage = seer::buffer.usage();
		REQUIRE(seer::buffer.usage().total_in_bytes == (2 * sizeof(seer::internal::DataPoint)));
	}
}
