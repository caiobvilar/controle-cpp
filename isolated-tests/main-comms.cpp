#include "communication.hpp"
#define SERVER "127.0.0.1"
#deifne PORT 3490

int main()
{
	Communication test = new Communication(SERVER,PORT);
	while(true)
	{
		std::cout << "GOT: " << test->getfrom_rcv_queue() << std::endl;
		sleep(1);
	}
	delete test;
	return 0;
}
