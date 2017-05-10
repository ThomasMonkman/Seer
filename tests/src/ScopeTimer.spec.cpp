#include "catch\catch.hpp"

#include "Seer.hpp"

#include "PipeSpy.hpp"

TEST_CASE("ScopeTimer sends data on to network", "[scope_timer]") 
{
	Seer::Sink<PipeSpy> sink;
	auto pipe_spy = sink.get_sink().lock();
	pipe_spy->get_match("tp").get();
	SECTION("ScopeTimer sends to network") 
	{
		Seer::ScopeTimer("test");
	}
}