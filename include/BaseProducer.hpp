#ifndef BASEPRODUCER_HPP
#define BASEPRODUCER_HPP
namespace Seer {
	class BaseProducer
	{
	public:
		BaseProducer();
		~BaseProducer();
		int hello;
	private:
		//Send data to network
		void send();
	};
}
#endif