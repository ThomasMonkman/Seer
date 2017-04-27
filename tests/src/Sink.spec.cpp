#include "catch\catch.hpp"
#include "Sink.hpp"

class DummySink : Seer::BaseSink
{
public:
	DummySink();
	~DummySink();
	void send(const std::string& data) override
	{}
private:

};

DummySink::DummySink()
{
}

DummySink::~DummySink()
{
}

TEST_CASE("Custom sinks can be created", "[sink]") {
	Seer::Sink<DummySink> dummy_sink();
}