#include "../Seer.hpp"

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench/picobench.hpp"


static void scope_timer(picobench::state& s)
{
	seer::buffer.clear();
	for (auto _ : s)
	{
		seer::ScopeTimer("test");
	}
	seer::dump_to_file();
}
PICOBENCH(scope_timer);