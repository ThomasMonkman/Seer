#include "catch\catch.hpp"

#include "json\json.hpp"

#include "Seer.hpp"

#include "PipeSpy.hpp"

TEST_CASE("ScopeTimer sends data on to network", "[scope_timer]") 
{		
	//PipeSpy sink;
	Seer::Sink<PipeSpy> spy_sink;
	auto spy = spy_sink.get_sink();
	auto construction_promise = spy->get_match("tp");
	std::future<nlohmann::json> destruction_promise;
	SECTION("ScopeTimer sends to network") 
	{
		Seer::ScopeTimer("test");
		destruction_promise = spy->get_match("tp");
		//construction_json;
	}
	try
	{
		auto json_c = construction_promise.get();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	auto json_d = destruction_promise.get();
}