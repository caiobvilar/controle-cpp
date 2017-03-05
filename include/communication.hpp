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
#include <iostream>

extern "C"
{
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
}

template<typename T>

class Data
{
	time_t timestamp;
	T datum;
};

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

		static std::queue<std::string> send_queue;		//queue that sends that to control the plant
		static std::queue<std::string> recv_queue;		//queue that receives data from server
		static std::thread recv_thread;								//thread that monitors, and manages the queues
		static std::thread send_thread;								//thread that monitors, and manages the queues
		static std::mutex sndmtx_queue;								//Mutex to grant shared access to the recv queue
		static std::mutex rcvmtx_queue;								//Mutex to grant shared access to the recv queue
		int fdread;																				//socket file descriptor
		int fdwrite;																				//socket file descriptor
		std::string portread;															//server port to connect
		std::string portwrite;															//server port to connect
		std::string srv_ip;														//server ip to connect
		std::string sndstring;
		std::string recvstring;

		bool connected;
		struct addrinfo server_addr;
		struct addrinfo *server;

		//////////////METHODS////////////////////////
		
		int StartComms();
		int EndComms();
		static void RunSnd();
		static void RunRcv();
	public:
		Communication(std::string ip, std::string port);
		~Communication();
		void insert_snd_queue(std::string);
		std::string getfrom_rcv_queue();
};
//TODO
// 1 - The queues should be of type Data, so they are interchangeable through the whole code
// 2 - 

#endif //_COMMUNICATION_HPP
