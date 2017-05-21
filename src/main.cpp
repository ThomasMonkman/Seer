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

class DummySinkA : public Seer::BaseSink
{
public:
	DummySinkA() {
	}
	~DummySinkA() override {
	}
	void send(const std::string& data) override
	{
		std::cout << "Dummy Sink got data:\n" << data << '\n';
	}
private:

};

//#include <>
int main() 
{
	auto default_number_of_sinks = Seer::Pipe::instance().number_of_sinks_attached();
	Seer::Sink<DummySinkA> dummy_sink_a;

	////Seer::BaseProducer base = Seer::BaseProducer();
	//std::mt19937_64 rand_eng{ std::random_device{}() };  // or seed however you want
	//std::uniform_int_distribution<> dist{ 1000, 5000 };
	while (true)
	{
			Seer::ScopeTimer timer("hello");
		std::this_thread::sleep_for(std::chrono::milliseconds{ 5000 });
	}
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return 0;
}