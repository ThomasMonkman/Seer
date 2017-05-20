#include "catch\catch.hpp"
#include "json\json.hpp"

#include "Seer.hpp"

#include "PipeSpy.hpp"
#include "TestHelper.hpp"

TEST_CASE("ScopeTimer sends data on to network", "[scope_timer]")
{
	Seer::Sink<PipeSpy> spy_sink;
	auto spy = spy_sink.get_sink();
	auto construction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["s"] == 1;
	});
	auto destruction_promise = spy->get_match([](const nlohmann::json& json) {
		return json["#"] == "tp" && json["s"] == 0;
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

	REQUIRE(json_c["t_id"] == json_c["t_id"]);
}