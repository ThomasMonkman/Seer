#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

void isCompleteEvent(const nlohmann::json& event, const std::string& name) {
	REQUIRE(event["name"] == name);
	REQUIRE(event["ph"] == "X");
	REQUIRE(event["ts"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["dur"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["pid"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["tid"].type() == nlohmann::json::value_t::number_unsigned);
}

void isInstantEvent(const nlohmann::json& event, const std::string& name, seer::internal::InstantEventScope type) {
	REQUIRE(event["name"] == name);
	REQUIRE(event["ph"] == "i");
	REQUIRE(event["ts"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["pid"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["tid"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["s"].type() == nlohmann::json::value_t::string);
	REQUIRE(event["s"] == std::string(1, static_cast<char>(type)));
}

void isCounterEvent(const nlohmann::json& event, const std::string& name) {
	REQUIRE(event["name"] == name);
	REQUIRE(event["ph"] == "C");
	REQUIRE(event["ts"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["pid"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["tid"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["args"].type() == nlohmann::json::value_t::object);
	REQUIRE(event["args"].find(name) != event["args"].end());
}

template<typename T>
T getCounterEventData(const nlohmann::json& event) {
	return event["args"][event["name"].get<std::string>()].get<T>();
}

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

TEST_CASE("ScopeTimer produces correct json", "[ScopeTimer]") {

	seer::buffer.clear();

	SECTION("0 event") {
		REQUIRE(seer::buffer.str() == "[]");
	}

	SECTION("1 event") {
		{
			seer::ScopeTimer test("Test");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		isCompleteEvent(json[0], "Test");
	}

	SECTION("2 events") {
		{
			seer::ScopeTimer test("Test");
			seer::ScopeTimer test2("Test2");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 2);
		isCompleteEvent(json[0], "Test2");
		isCompleteEvent(json[1], "Test");
	}
}


TEST_CASE("InstantEvent produces correct json", "[seer::instant_event]") {

	seer::buffer.clear();

	SECTION("0 event") {
		REQUIRE(seer::buffer.str() == "[]");
	}

	SECTION("default") {
		{
			seer::instant_event("Test");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		isInstantEvent(json[0], "Test", seer::internal::InstantEventScope::thread);
	}
	
	SECTION("1 event") {
		{
			seer::instant_event("Test");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		isInstantEvent(json[0], "Test", seer::internal::InstantEventScope::thread);
	}

	SECTION("2 events") {
		{
			seer::instant_event("Test");
			seer::instant_event("Test2");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 2);
		isInstantEvent(json[0], "Test", seer::internal::InstantEventScope::thread);
		isInstantEvent(json[1], "Test2", seer::internal::InstantEventScope::thread);
	}

	SECTION("scope == global") {
		{
			seer::instant_event("Test", seer::internal::InstantEventScope::global);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		isInstantEvent(json[0], "Test", seer::internal::InstantEventScope::global);
	}
	SECTION("scope == process") {
		{
			seer::instant_event("Test", seer::internal::InstantEventScope::process);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		isInstantEvent(json[0], "Test", seer::internal::InstantEventScope::process);
	}
	SECTION("scope == thread") {
		{
			seer::instant_event("Test", seer::internal::InstantEventScope::thread);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		isInstantEvent(json[0], "Test", seer::internal::InstantEventScope::thread);
	}
}


TEST_CASE("Count produces correct json", "[seer::Count]") {

	seer::buffer.clear();

	SECTION("0 event") {
		REQUIRE(seer::buffer.str() == "[]");
	}

	SECTION("1 event") {
		{
			seer::Counter<int> counter("Test", 0);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		isCounterEvent(json[0], "Test");
		REQUIRE(getCounterEventData<int>(json[0]) == 0);
	}

	SECTION("2 events") {
		{
			seer::Counter<int> counter("Test", 0);
			seer::Counter<int> counter2("Test2", 1);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 2);
		isCounterEvent(json[0], "Test");
		REQUIRE(getCounterEventData<int>(json[0]) == 0);
		isCounterEvent(json[1], "Test2");
		REQUIRE(getCounterEventData<int>(json[1]) == 1);
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
		
		
		for (auto i = 0; i < test_case.size(); i++)
		{
			isCounterEvent(json[i], "Test");
			REQUIRE(getCounterEventData<int>(json[i]) == test_case[i]);
		}
	}
}
