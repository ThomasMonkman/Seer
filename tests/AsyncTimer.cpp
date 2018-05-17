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
			const auto timer = async.create_timer("Step 1", seer::Async::LifeTime::start);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		test_helper::get_with_timeout(std::async(std::launch::async, [async] {
			auto timer2 = async.create_timer("Step 2", seer::Async::LifeTime::continued);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}));
		{
			const auto timer = async.create_timer("Step 1", seer::Async::LifeTime::start);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		/*seer::AsyncTimer asyncTimer;
		{
			auto timer = asyncTimer.Time("Test", AsyncTimer::continue);
			
			
			seer::AsyncTimer test("Test");
			id = test.continue();
		}
		{
			auto timer = asyncTimer.Time("Test2", AsyncTimer::end);
			
			2 time
			
			asyncTimer.end();
			
			
			id.newTimer("Test2")
			seer::AsyncTimer test("Test2", id);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		is_complete_event(json[0], "Test");*/
	}
}
