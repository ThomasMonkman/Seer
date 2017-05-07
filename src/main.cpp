//#include <3rdParty\json\json.hpp>
//#include <..\..\3rdParty\json\json.hpp>
//#include "..\"

//#include <include\BaseProducer.hpp>
//#include "../include/BaseProducer.hpp"

#include "Seer.hpp"

//#include "ScopeTimer.hpp"
//#include "Sink.hpp"

#include <limits>
#include <string>
#include <thread>
#include <chrono>
#include <random>

class DummySinkA : Seer::BaseSink
{
public:
	DummySinkA() {
		std::cout << "DummySinkA Created";
	}
	~DummySinkA() override {
		std::cout << "DummySinkA Destroyed";
	}
	void send(const std::string& data) override
	{}
private:

};

//#include <>
int main() {
	/*nlohmann::json json = {
		{"a", 1}
	};*/
	//Seer::BaseProducer base = Seer::BaseProducer();
	std::mt19937_64 rand_eng{ std::random_device{}() };  // or seed however you want
	std::uniform_int_distribution<> dist{ 1000, 5000 };
	Seer::Sink<DummySinkA> dummy_sink;
	while (true)
	{
		for (auto i = 0; i < 1; i++)
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