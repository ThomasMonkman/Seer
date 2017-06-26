#include "catch/catch.hpp"
#include "json/json.hpp"

#include "Seer.hpp"

#include "Util/PipeSpy.hpp"
#include "Util/TestHelper.hpp"

#include <array>
#include <algorithm>

TEST_CASE("Timer sends data on to network", "[timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	const auto spy = spy_sink.get_sink();
	SECTION("Timer sends on tick")
	{
		auto tick_promise = spy->get_match([](const nlohmann::json& json) {
			return json["#"] == "tp" && json["p"] == 0 && json["n"] == "test1";
		});
		auto timer = Seer::Timer("test1");
		timer.tick();
		auto json_tick = TestHelper::get_with_timeout<nlohmann::json>(tick_promise);
	}
}

TEST_CASE("Timer destructs correctly", "[timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	const auto spy = spy_sink.get_sink();
	auto destruction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["l"] == true;
	});
	SECTION("Timer sends final on destruction")
	{
		{
			auto timer = Seer::Timer("test2");
			timer.tick();
		}
		auto json_destruction = TestHelper::get_with_timeout<nlohmann::json>(destruction_promise);
		REQUIRE(json_destruction["p"] == 1);
	}
	SECTION("Timer sends final on destruction even if tick is not called")
	{
		{
			auto timer = Seer::Timer("test3");
		}
		auto json_destruction = TestHelper::get_with_timeout<nlohmann::json>(destruction_promise);
		REQUIRE(json_destruction["p"] == 0);
	}
}


TEST_CASE("Timer sends the correct data", "[timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	const auto spy = spy_sink.get_sink();
	auto tick_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 0 && json["n"] == "test4";
	});
	SECTION("Timer sends to network")
	{
		auto timer = Seer::Timer("test4");
		timer.tick();
		auto json_tick = TestHelper::get_with_timeout<nlohmann::json>(tick_promise);
		REQUIRE(json_tick["n"] == "test4");
		REQUIRE(json_tick["p"] == 0);
		REQUIRE(json_tick["l"] == false);
	}
}


TEST_CASE("Timer can continuously send data", "[timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	const auto spy = spy_sink.get_sink();
	const auto tick_number = 10;
	std::array<std::future<nlohmann::json>, tick_number> tick_promises;
	auto pos = 0;
	for (auto && tick_promise : tick_promises)
	{
		tick_promise = spy->get_match([pos](const nlohmann::json& json) {
			return json["#"] == "tp" && json["p"] == pos && json["n"] == "test5";
		});
		pos++;
	}
	SECTION("Timer sends to network")
	{
		auto timer = Seer::Timer("test5");
		for (auto i = 0; i < tick_number; i++)
		{
			timer.tick();
			auto json_tick = TestHelper::get_with_timeout<nlohmann::json>(tick_promises[i]);
			REQUIRE(json_tick["n"] == "test5");
			REQUIRE(json_tick["p"] == i);
			REQUIRE(json_tick["l"] == false);
		}
	}
}