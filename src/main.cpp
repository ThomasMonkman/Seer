//#include <3rdParty\json\json.hpp>
//#include <..\..\3rdParty\json\json.hpp>
//#include "..\"

//#include <include\BaseProducer.hpp>
//#include "../include/BaseProducer.hpp"
#include "ScopeTimer.hpp"

#include <limits>
#include <BaseProducer.hpp>
#include <string>
#include <thread>
#include <chrono>
#include <random>
//#include <>
int main() {
	/*nlohmann::json json = {
		{"a", 1}
	};*/
	//Seer::BaseProducer base = Seer::BaseProducer();
	std::mt19937_64 rand_eng{ std::random_device{}() };  // or seed however you want
	std::uniform_int_distribution<> dist{ 1000, 5000 };

	while (true)
	{
		for (auto i = 0; i < 1000; i++)
		{
			Seer::ScopeTimer timer("hello");
			//std::cout << dist(rand_eng) << '\n';
			//std::this_thread::sleep_for(std::chrono::milliseconds{ dist(rand_eng) });		
		}
		std::this_thread::sleep_for(std::chrono::milliseconds{ 5000 });
	}
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return 0;
}