#include "communication.hpp"
#define SERVER "192.168.0.28"
//#define SERVER "127.0.0.1"
#define PORT 4001
#define ADCHANNEL	4
#define DACCHANNEL 4

int main()
{
	Communication *test = new Communication(SERVER,PORT,ADCHANNEL,DACCHANNEL);
	std::cout << "[DEBUG]: instantiated Communication." << std::endl;
	while(test->running)
	{
		std::cout << "GOT: " << test->getfrom_rcv_queue() << "Running: "<< test->running << std::endl;
		usleep(300000);
	}
	delete test;
	std::cout << "[DEBUG]: deleted Communication instance." << std::endl;
	return 0;
}
