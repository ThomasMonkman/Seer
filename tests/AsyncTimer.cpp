#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

#include <thread>
#include <chrono>

//void is_complete_event(const nlohmann::json& event, const std::string& name) {
//	REQUIRE(event["name"] == name);
//	REQUIRE(event["ph"] == "X");
//	REQUIRE(event["ts"].type() == nlohmann::json::value_t::number_unsigned);
//	REQUIRE(event["dur"].type() == nlohmann::json::value_t::number_unsigned);
//	REQUIRE(event["pid"].type() == nlohmann::json::value_t::number_unsigned);
//	REQUIRE(event["tid"].type() == nlohmann::json::value_t::string);
//}

TEST_CASE("Async produces correct json", "[ScopeTimer]") {

	test_helper::reset_seer();

	SECTION("0 event") {
		REQUIRE(seer::buffer.str() == "[]");
	}

	SECTION("1 event") {

		seer::Async async;
		{
			const auto timer = async.create_timer("Step 1");
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		test_helper::get_with_timeout<void>(std::async(std::launch::async, [async] {
			const auto timer2 = async.create_timer("Step 2");
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}));
		{
			const auto timer3 = async.create_timer("Step 3");
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 6);
	}
}
