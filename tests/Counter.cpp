#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

void is_counter_event(const nlohmann::json& event, const std::string& name) {
	REQUIRE(event["name"] == name);
	REQUIRE(event["ph"] == "C");
	REQUIRE(event["ts"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["pid"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["tid"].type() == nlohmann::json::value_t::string);
	REQUIRE(event["args"].type() == nlohmann::json::value_t::object);
	REQUIRE(event["args"].find(name) != event["args"].end());
}

template<typename T>
T get_counter_event_data(const nlohmann::json& event) {
	return event["args"][event["name"].get<std::string>()].get<T>();
}

TEST_CASE("Count produces correct json", "[seer::Count]") {

	test_helper::reset_seer();

	SECTION("0 event") {
		REQUIRE(seer::buffer.str() == "[]");
	}

	SECTION("1 event") {
		{
			seer::Counter<int> counter("Test", 0);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		is_counter_event(json[0], "Test");
		REQUIRE(get_counter_event_data<int>(json[0]) == 0);
	}

	SECTION("2 events") {
		{
			seer::Counter<int> counter("Test", 0);
			seer::Counter<int> counter2("Test2", 1);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 2);
		is_counter_event(json[0], "Test");
		REQUIRE(get_counter_event_data<int>(json[0]) == 0);
		is_counter_event(json[1], "Test2");
		REQUIRE(get_counter_event_data<int>(json[1]) == 1);
	}

	SECTION("1 event multiple times") {
		auto test_case = std::vector<int>{ 0, 2, 4, 5 };
		{
			for (const auto test : test_case) 
			{
				seer::Counter<int> counter("Test", test);
			}
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == test_case.size());
				
		for (auto i = 0u; i < test_case.size(); i++)
		{
			is_counter_event(json[i], "Test");
			REQUIRE(get_counter_event_data<int>(json[i]) == test_case[i]);
		}
	}

	SECTION("update works") {
		auto test_case = std::vector<int>{ 0, 2, 4, 5 };
		{
			seer::Counter<int> counter("Test", 9);
			for (const auto test : test_case)
			{
				counter.update(test);
			}
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == test_case.size() + 1);

		for (auto i = 1u; i < test_case.size() + 1; i++)
		{
			is_counter_event(json[i], "Test");
			REQUIRE(get_counter_event_data<int>(json[i]) == test_case[i - 1]);
		}
	}

	SECTION("bool") {
		{
			seer::Counter<bool> counter("Test", false);
			counter.update(true);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 2);
		is_counter_event(json[0], "Test");
		REQUIRE(get_counter_event_data<bool>(json[0]) == false);
		is_counter_event(json[1], "Test");
		REQUIRE(get_counter_event_data<bool>(json[1]) == true);
	}
}