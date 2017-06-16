#include "catch/catch.hpp"
#include "json/json.hpp"

#include "Seer.hpp"

#include "Util/PipeSpy.hpp"
#include "Util/TestHelper.hpp"

TEST_CASE("ScopeTimer sends data on to network", "[scope_timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	auto spy = spy_sink.get_sink();
	auto construction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 0;
	});
	auto destruction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 1;
	});
	SECTION("ScopeTimer sends to network")
	{
		Seer::ScopeTimer("test");
		auto json_construction = TestHelper::get_with_timeout<nlohmann::json>(construction_promise);
	}
	auto json_destruction = TestHelper::get_with_timeout<nlohmann::json>(destruction_promise);
}


TEST_CASE("ScopeTimer sends the correct data", "[scope_timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	auto spy = spy_sink.get_sink();
	auto construction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 0;
	});
	auto destruction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 1;
	});
	nlohmann::json json_construction;
	SECTION("ScopeTimer sends to network")
	{
		Seer::ScopeTimer("test");
		json_construction = TestHelper::get_with_timeout<nlohmann::json>(construction_promise);
	}
	auto json_destruction = TestHelper::get_with_timeout<nlohmann::json>(destruction_promise);
	// contrustion
	REQUIRE(json_construction["n"] == "test");
	REQUIRE(json_construction["p"] == 0);
	REQUIRE(json_construction["l"] == false);

	// destruction
	REQUIRE(json_destruction["n"] == "test");
	REQUIRE(json_destruction["p"] == 1);
	REQUIRE(json_destruction["l"] == true);

	// related fields
	REQUIRE(json_construction["t_id"] == json_destruction["t_id"]);
}

TEST_CASE("ScopeTimer is isolated by threads", "[scope_timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	auto spy = spy_sink.get_sink();
	auto time_point_promise = [](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 0;
	};
	SECTION("when both ScopeTimers have the same name")
	{
		auto thread_1_promise = spy->get_match(time_point_promise);
		auto thread_1 = std::thread([]() { Seer::ScopeTimer("test"); });
		auto thread_1_json = TestHelper::get_with_timeout<nlohmann::json>(thread_1_promise);

		auto thread_2_promise = spy->get_match(time_point_promise);
		auto thread_2 = std::thread([]() { Seer::ScopeTimer("test"); });
		auto thread_2_json = TestHelper::get_with_timeout<nlohmann::json>(thread_2_promise);

		thread_1.join();
		thread_2.join();

		REQUIRE(thread_1_json["t_id"] != thread_2_json["t_id"]);
	}

	SECTION("when both ScopeTimers have different names")
	{
		auto thread_1_promise = spy->get_match(time_point_promise);
		auto thread_1 = std::thread([]() { Seer::ScopeTimer("test1"); });
		auto thread_1_json = TestHelper::get_with_timeout<nlohmann::json>(thread_1_promise);

		auto thread_2_promise = spy->get_match(time_point_promise);
		auto thread_2 = std::thread([]() { Seer::ScopeTimer("test2"); });
		auto thread_2_json = TestHelper::get_with_timeout<nlohmann::json>(thread_2_promise);

		thread_1.join();
		thread_2.join();

		REQUIRE(thread_1_json["t_id"] != thread_2_json["t_id"]);
	}
}
