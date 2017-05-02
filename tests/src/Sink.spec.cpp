#include "catch\catch.hpp"
#include "Pipe.hpp"
#include "Sink.hpp"


class DummySink : Seer::BaseSink
{
public:
	DummySink() {
		std::cout << "DummySink Created";
	}
	~DummySink() override {
		std::cout << "DummySink Destroyed";
	}
	void send(const std::string& data) override
	{}
private:

};

TEST_CASE("Custom sinks can be created", "[sink]") {
	auto default_number_of_sinks = Seer::Pipe::instance().number_of_sinks_attached();
	SECTION("Sink is added to pipe") {	
		REQUIRE(Seer::Pipe::instance().number_of_sinks_attached() == default_number_of_sinks);
		Seer::Sink<DummySink> dummy_sink;
		REQUIRE(Seer::Pipe::instance().number_of_sinks_attached() == default_number_of_sinks + 1);
	}
	SECTION("Sink is removed from pipe on destruction") {
		{	
			Seer::Sink<DummySink> dummy_sink;
			REQUIRE(Seer::Pipe::instance().number_of_sinks_attached() == default_number_of_sinks + 1);
		}
		REQUIRE(Seer::Pipe::instance().number_of_sinks_attached() == default_number_of_sinks);
	}
}