#ifndef NETWORK_HPP
#define NETWORK_HPP
namespace Seer {
	class Network
	{
	public:
		Network();
		~Network();
	private:
		//Send data to network
		void send();
	};
}
#endif