#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

TEST_CASE("dummy", "[dummy]") {
	{
		seer::ScopeTimer test("Time");
		for (size_t i = 0; i < 100000; i++)
		{
			seer::ScopeTimer test("+-");
		}
	}
	seer::dump_to_file();
	//std::cout << seer::buffer << std::endl;
	//std::string s = seer::buffer.str();
	REQUIRE(false == true);
}
