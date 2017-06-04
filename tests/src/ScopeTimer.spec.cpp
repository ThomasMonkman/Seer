#include "catch/catch.hpp"
#include "json/json.hpp"

#include "Seer.hpp"

#include "PipeSpy.hpp"
#include "TestHelper.hpp"

TEST_CASE("ScopeTimer sends data on to network", "[scope_timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	auto spy = spy_sink.get_sink();
	auto construction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 1;
	});
	auto destruction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 0;
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
		return json["#"] == "tp" && json["p"] == 1;
	});
	auto destruction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 0;
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

	// destruction
	REQUIRE(json_destruction["n"] == "test");
	REQUIRE(json_construction["p"] == 1);

	// related fields
	REQUIRE(json_construction["t_id"] == json_destruction["t_id"]);
}

TEST_CASE("ScopeTimer is isolated by threads", "[scope_timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	auto spy = spy_sink.get_sink();
	auto construction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 1;
	});
	auto destruction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["p"] == 0;
	});
	nlohmann::json json_c;
	SECTION("ScopeTimer sends to network")
	{
		Seer::ScopeTimer("test");
		json_c = TestHelper::get_with_timeout<nlohmann::json>(construction_promise);
		REQUIRE(json_c["n"] == "test");
	}
	auto json_d = TestHelper::get_with_timeout<nlohmann::json>(destruction_promise);
	REQUIRE(json_d["n"] == "test");

	REQUIRE(json_c["t_id"] == json_d["t_id"]);
}
