#include "catch/catch.hpp"

#include "../Seer.hpp"

#include "json/json.hpp"

#include "Util/TestHelper.hpp"

#include <iostream>

void is_instant_event(const nlohmann::json& event, const std::string& name, seer::internal::InstantEventScope type) {
	REQUIRE(event["name"] == name);
	REQUIRE(event["ph"] == "i");
	REQUIRE(event["ts"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["pid"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["tid"].type() == nlohmann::json::value_t::number_unsigned);
	REQUIRE(event["s"].type() == nlohmann::json::value_t::string);
	REQUIRE(event["s"] == std::string(1, static_cast<char>(type)));
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
		is_instant_event(json[0], "Test", seer::internal::InstantEventScope::thread);
	}
	
	SECTION("1 event") {
		{
			seer::instant_event("Test");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		is_instant_event(json[0], "Test", seer::internal::InstantEventScope::thread);
	}

	SECTION("2 events") {
		{
			seer::instant_event("Test");
			seer::instant_event("Test2");
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 2);
		is_instant_event(json[0], "Test", seer::internal::InstantEventScope::thread);
		is_instant_event(json[1], "Test2", seer::internal::InstantEventScope::thread);
	}

	SECTION("scope == global") {
		{
			seer::instant_event("Test", seer::internal::InstantEventScope::global);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		is_instant_event(json[0], "Test", seer::internal::InstantEventScope::global);
	}
	SECTION("scope == process") {
		{
			seer::instant_event("Test", seer::internal::InstantEventScope::process);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		is_instant_event(json[0], "Test", seer::internal::InstantEventScope::process);
	}
	SECTION("scope == thread") {
		{
			seer::instant_event("Test", seer::internal::InstantEventScope::thread);
		}
		const auto json = nlohmann::json::parse(seer::buffer.str());
		REQUIRE(json.size() == 1);
		is_instant_event(json[0], "Test", seer::internal::InstantEventScope::thread);
	}
}