#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

TEST_CASE("buffer works", "[seer::buffer]") {

	seer::buffer.clear();

	SECTION("start empty") {
		REQUIRE(seer::buffer.usage().usage_in_bytes == 0);
		REQUIRE(seer::buffer.usage().percent_used() == 0.0);
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
		REQUIRE(seer::buffer.usage().percent_used() != 0.0);
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
}
