#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

TEST_CASE("buffer clears", "[seer::buffer]") {

	seer::buffer.clear();

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
}
