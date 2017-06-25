#include "catch/catch.hpp"
#include "json/json.hpp"

#include "Seer.hpp"

#include "Util/PipeSpy.hpp"
#include "Util/TestHelper.hpp"

TEST_CASE("Timer sends data on to network", "[timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	auto spy = spy_sink.get_sink();
	SECTION("Timer sends on tick")
	{
		auto tick_promise = spy->get_match([](const nlohmann::json& json) {
			return json["#"] == "tp" && json["p"] == 0;
		});
		auto timer = Seer::Timer("test");
		timer.tick();
		auto json_tick = TestHelper::get_with_timeout<nlohmann::json>(tick_promise);
	}
	SECTION("Timer sends final on destruction")
	{
		auto destruction_promise = spy->get_match([](const nlohmann::json& json) {
			return json["#"] == "tp" && json["l"] == true;
		});
		{
			auto timer = Seer::Timer("test");
			timer.tick();
		}
		auto json_destruction = TestHelper::get_with_timeout<nlohmann::json>(destruction_promise);
		REQUIRE(json_destruction["p"] == 1);
	}
	SECTION("Timer sends final on destruction even if tick is not called")
	{
		auto destruction_promise = spy->get_match([](const nlohmann::json& json) {
			return json["#"] == "tp" && json["l"] == true;
		});
		{
			auto timer = Seer::Timer("test");
		}
		auto json_destruction = TestHelper::get_with_timeout<nlohmann::json>(destruction_promise);
		REQUIRE(json_destruction["p"] == 0);
	}
}


TEST_CASE("Timer sends the correct data", "[timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	auto spy = spy_sink.get_sink();
	auto tick_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 0;
	});
	SECTION("Timer sends to network")
	{
		auto timer = Seer::Timer("test");
		timer.tick();
		auto json_tick = TestHelper::get_with_timeout<nlohmann::json>(tick_promise);
		REQUIRE(json_tick["n"] == "test");
		REQUIRE(json_tick["p"] == 0);
		REQUIRE(json_tick["l"] == false);
	}	
}


TEST_CASE("Timer can continuously send data", "[timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	auto spy = spy_sink.get_sink();
	auto tick_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 0;
	});
	SECTION("Timer sends to network")
	{
		auto timer = Seer::Timer("test");
		timer.tick();
		auto json_tick = TestHelper::get_with_timeout<nlohmann::json>(tick_promise);
		REQUIRE(json_tick["n"] == "test");
		REQUIRE(json_tick["p"] == 0);
		REQUIRE(json_tick["l"] == false);
	}
}