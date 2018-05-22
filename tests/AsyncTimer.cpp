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

	SECTION("1 event - 3 steps") {

		seer::Async async;
		{
			const auto timer = async.create_timer("a.1");
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
		test_helper::get_with_timeout<void>(std::async(std::launch::async, [async] {
			const auto timer2 = async.create_timer("a.2");
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}));
		{
			const auto timer3 = async.create_timer("a.3");
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}

		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 6);
		{
			nlohmann::json flow_starts;
			std::copy_if(json.begin(), json.end(), std::back_inserter(flow_starts), [](const nlohmann::json& e) {return e["ph"] == "s"; });
			REQUIRE(flow_starts.size() == 1);
		}
		{
			nlohmann::json flow_steps;
			std::copy_if(json.begin(), json.end(), std::back_inserter(flow_steps), [](const nlohmann::json& e) {return e["ph"] == "t"; });
			REQUIRE(flow_steps.size() == 2);
		}
		{
			nlohmann::json flow_events;
			std::copy_if(json.begin(), json.end(), std::back_inserter(flow_events), [](const nlohmann::json& e) {return e["ph"] == "t" || e["ph"] == "s"; });
			REQUIRE(flow_events.size() == 3);
			nlohmann::json flow_events_a;
			std::copy_if(flow_events.begin(), flow_events.end(), std::back_inserter(flow_events_a), [](const nlohmann::json& e) {return e["id"] == "0"; });
			REQUIRE(flow_events_a.size() == 3);

			//check the first flow event in chronological order is a start flow event
			std::sort(flow_events_a.begin(), flow_events_a.end(), [](const nlohmann::json& a, const nlohmann::json& b) {return a["ts"] < b["ts"]; });
			REQUIRE(flow_events_a[0]["ph"] == "s");
		}
	}

	SECTION("2 events - 3 steps") {

		seer::Async async_a;
		seer::Async async_b;
		{
			const auto timer_a = async_a.create_timer("a.1");
			const auto timer_b = async_b.create_timer("b.1");
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
		test_helper::get_with_timeout<void>(std::async(std::launch::async, [async_a, async_b] {
			const auto timer2_a = async_a.create_timer("a.2");
			const auto timer2_b = async_b.create_timer("b.2");
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}));
		{
			const auto timer3_a = async_a.create_timer("a.3");
			const auto timer3_b = async_b.create_timer("b.3");
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}

		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 12);
		{
			nlohmann::json flow_starts;
			std::copy_if(json.begin(), json.end(), std::back_inserter(flow_starts), [](const nlohmann::json& e) {return e["ph"] == "s"; });
			REQUIRE(flow_starts.size() == 2);
		}
		{
			nlohmann::json flow_steps;
			std::copy_if(json.begin(), json.end(), std::back_inserter(flow_steps), [](const nlohmann::json& e) {return e["ph"] == "t"; });
			REQUIRE(flow_steps.size() == 4);
		}
		{
			nlohmann::json flow_events;
			std::copy_if(json.begin(), json.end(), std::back_inserter(flow_events), [](const nlohmann::json& e) {return e["ph"] == "t" || e["ph"] == "s"; });
			REQUIRE(flow_events.size() == 6);
			nlohmann::json flow_events_a;
			std::copy_if(flow_events.begin(), flow_events.end(), std::back_inserter(flow_events_a), [](const nlohmann::json& e) {return e["id"] == "1"; });
			REQUIRE(flow_events_a.size() == 3);
			nlohmann::json flow_events_b;
			std::copy_if(flow_events.begin(), flow_events.end(), std::back_inserter(flow_events_b), [](const nlohmann::json& e) {return e["id"] == "2"; });
			REQUIRE(flow_events_b.size() == 3);

			//check the first flow event in chronological order is a start flow event
			std::sort(flow_events_a.begin(), flow_events_a.end(), [](const nlohmann::json& a, const nlohmann::json& b) {return a["ts"] < b["ts"]; });
			REQUIRE(flow_events_a[0]["ph"] == "s");
			std::sort(flow_events_b.begin(), flow_events_b.end(), [](const nlohmann::json& a, const nlohmann::json& b) {return a["ts"] < b["ts"]; });
			REQUIRE(flow_events_b[0]["ph"] == "s");
		}
	}
}
