#ifndef _COMMUNICATION_HPP
#define _COMMUNICATION_HPP

///////////////////////INCLUDES//////////////////
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <string>
#include <ctime>
#include <chrono>
#include <iostream>

#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>


//Todas as mensagens com a placa de comunicacao devem ser da forma
//ESCREVER TENSAO
//"WRITE" + " " + CHANNEL + " " + VOLTS + "\n"
//Esperar "ACK"
//LER TENSAO
//"READ" + CHANNEL + "\n"

class Communication
{
	private:

		/////////////MEMBERS/////////////////////////

		std::queue<std::string> send_queue;		//queue that sends that to control the plant
		std::queue<std::string> recv_queue;		//queue that receives data from server
		std::thread recv_thread;							//thread that monitors, and manages the queues
		std::thread send_thread;							//thread that monitors, and manages the queues
		std::mutex sndmtx_queue;							//Mutex to grant shared access to the recv queue
		std::mutex rcvmtx_queue;							//Mutex to grant shared access to the recv queue
		std::mutex stopmtx_threads;						//Mutex to grant shared access to stopthread variable
		int fdread;														//socket file descriptor
		int fdwrite;													//socket file descriptor
		int port;															//server port to connect
		std::string srv_ip;										//server ip to connect
		std::string sndstring;

		bool stopthreads;
		struct addrinfo server_addr;
		struct addrinfo *server;

		//////////////METHODS////////////////////////
		int resolveHostname(const char *hostname,struct in_addr *addr);
		int StartConn();
		int EndConn(int);
		void RunSnd();
		void RunRcv();
		void Start();
		void Stop();
		void setstopthreads(bool);
		bool isstopped();
	public:
		Communication(std::string ip,int port);
		~Communication();
		void insert_snd_queue(std::string);
		std::string getfrom_rcv_queue();
};
//TODO
// 1 - The queues should be of type Data, so they are interchangeable through the whole code
// 2 - 

#endif //_COMMUNICATION_HPP
