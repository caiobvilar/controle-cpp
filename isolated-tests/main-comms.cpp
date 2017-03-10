#include "communication.hpp"
#define SERVER "10.13.99.69"
#define PORT 28001
#define ADCHANNEL	4
#define DACCHANNEL 4

int main()
{
	Communication *test = new Communication(SERVER,PORT,ADCHANNEL,DACCHANNEL);
	while(test->running)
	{
		std::cout << "GOT: " << test->getfrom_rcv_queue() << std::endl;
		sleep(1);
	}
	delete test;
	return 0;
}
