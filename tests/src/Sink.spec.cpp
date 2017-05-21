#include "catch\catch.hpp"

#include "Seer.hpp"
//#include "Pipe.hpp"
//#include "Sink.hpp"

#include <string>

class DummySink : public Seer::BaseSink
{
public:
	DummySink() {
		//std::cout << "DummySink Created";
	}
	~DummySink() override {
		//std::cout << "DummySink Destroyed";
	}
	void send(const std::string& data) override
	{
		//std::cout << data;
	}
};

class ArgumentWithInt : public Seer::BaseSink
{
public:
	ArgumentWithInt(int a) {
		//std::cout << a;
	}
	~ArgumentWithInt() override {
		//std::cout << "DummySink Destroyed";
	}
	void send(const std::string& data) override
	{
		//std::cout << data;
	}
};

class ArgumentWithMultipleInts : public Seer::BaseSink
{
public:
	ArgumentWithMultipleInts(int a, int b) {}
	~ArgumentWithMultipleInts() override {
		//std::cout << "DummySink Destroyed";
	}
	void send(const std::string& data) override
	{
		//std::cout << data;
	}
};

class ArgumentWithClass : public Seer::BaseSink
{
public:
	ArgumentWithClass(std::string a) {

	}
	~ArgumentWithClass() override {
		//std::cout << "DummySink Destroyed";
	}
	void send(const std::string& data) override
	{
		//std::cout << data;
	}
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
	SECTION("Sink can be constructed with arguments") {
		Seer::Sink<ArgumentWithInt> argument_with_int(1);
		Seer::Sink<ArgumentWithMultipleInts> argument_with_multiple_int(1, 2);
		Seer::Sink<ArgumentWithClass> argument_with_class(std::string("aa"));
	}
}