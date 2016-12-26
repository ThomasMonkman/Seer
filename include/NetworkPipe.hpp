namespace Seer {
	class BaseProducer
	{
	public:
		BaseProducer();
		~BaseProducer();
	private:
		//Send data to network
		void send();
	};
}