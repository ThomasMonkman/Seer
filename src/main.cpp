//#include <3rdParty\json\json.hpp>
//#include <..\..\3rdParty\json\json.hpp>
//#include "..\"

//#include <include\BaseProducer.hpp>
//#include "../include/BaseProducer.hpp"
#include <limits>
#include "ScopeTimer.hpp"
#include <BaseProducer.hpp>
#include <string>
//#include <>
int main() {
	/*nlohmann::json json = { 
		{"a", 1} 
	};*/
	//Seer::BaseProducer base = Seer::BaseProducer();
	{
		Seer::ScopeTimer timer("hello");
	}
	//std::cout << "Press Enter to Continue";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return 0;
}