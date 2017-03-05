#include "communication.hpp"

/////////////////////////UTILS///////////////////

int stoi(std::string str)
{
	std::istringstream aux(str);
	int res;
	aux >>res;
	return res;
}

std::string itos(int num)
{
	std::stringstream str;
	str << num;
	return str.str();
}

//converts structures to IPv6 ou IPv4 address families
void *get_int_addr(struct sockaddr *sockaddr)
{
	if(sockaddr->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sockaddr)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sockaddr)->sin6_addr);
}

///////////////////METHODS///////////////////////////////

int Communication::StartComms(int sock_fd,std::string port)
{
	char ip_data[INET6_ADDRSTRLEN];
	int rv; //return value to check for errors
	struct addrinfo *auxaddr; //auxiliary addrinfo struct to loop through addresses
	//clears addr_info structure to grant stability
	memset(&this->server_addr,0,sizeof(this->server_addr));
	//set host address family to IPv4
	this->server_addr.ai_family = AF_INET;

	//set host socket type to TCP
	this->server_addr.ai_socktype = SOCK_STREAM;

	//tries to get host information
	if((rv = getaddrinfo(this->srv_ip.c_str(),port.c_str(),&this->server_addr,&this->server)) != 0)
	{
		std::cerr << "[ERROR]: Failed to locate SERVER: " << this->srv_ip << "due to ["<< gai_strerror(rv)<< "]" << std::endl;
		return -1;
	}

	//Loop through all address regarding the given IP
	for(auxaddr = this->server; auxaddr != NULL;auxaddr = auxaddr->ai_next)
	{
		//tries to create socket
		if((sock_fd = socket(auxaddr->ai_family,auxaddr->ai_socktype,auxaddr->ai_protocol)) < 0)
		{
			std::cerr << "[ERROR]: Failed to initiate SOCKET." << std::endl;
			continue;
		}

		//connect to server
		if(connect(this->fd,auxaddr->ai_addr,auxaddr->ai_addrlen) == -1)
		{
			std::cerr << "[ERROR]: Failed to connect to server: " << this->srv_ip << std::endl;
			close(sock_fd);
			continue;
		}
		break;
	}
	if(auxaddr == NULL)
	{
		std::cerr << "[ERROR]: connection unsuccessful." << std::endl;
		return -1;
	}
	inet_ntop(auxaddr->ai_family, get_int_addr((struct sockaddr *)auxaddr->ai_addr),ip_data,sizeof(ip_data));
	std::cout << "[CLIENT]: connected succesfully to server [" << ip_data << "]" << std::endl;
	freeaddrinfo(this->server);
}

int Communication::EndComms()
{
	//close sockets
	close(this->fdread);
	close(this->fdwrite);
}


Communication::Communication(std::string ip, std::string port)
{
	this->connected = false;
	this->srv_ip = ip.c_str();
	this->port = port.c_str();
	if(this->StartComms() < 0)
	{
		std::cerr << "[Warning]: couldn`t connect to provided host.Errors above." << std::endl;
	}
	//start threads
	this->recv_thread = std::thread(&Communication::RunRcv);	//Start receiving queue management thread first
	this->send_thread = std::thread(&Communication::RunSnd);
}
Communication::~Communication()
{
	//Ends connection with the server
	this->EndComms();
	//Cleanses Queues
	this->sndmtx_queue.lock();
	while(!this->send_queue.empty())
	{
		this->send_queue.pop();  //purges oldest value from queue
	}
	this->sndmtx_queue.unlock();
	//Do I need to guarantee no concurrency on this queue too?
	while(!this->recv_queue.empty())
	{
		this->recv_queue.pop();
	}
	//Joins/Waits for threads to end
	this->recv_thread.join();
	this->send_thread.join();
}

void Communication::RunSnd()
{
	if(!send_queue.empty())
	{
		std::string snd_value;
		sndmtx_queue.lock();
		snd_value = send_queue.front();
		send_queue.pop();
		sndmtx_queue.unlock();
		//send();
	}

}
void Communication::RunRcv()
{
	rcvmtx_queue.lock();
	this->recv_queue.push(this->recvstring);
	rcvmtx_queue.unlock();
}

void Communication::insert_snd_queue(std::string data)
{
	this->sndmtx_queue.lock();
	this->send_queue.push(data);
	this->sndmtx_queue.unlock();
}

std::string Communication::getfrom_rcv_queue()
{
	std::string data;
	this->rcvmtx_queue.lock();
	data = this->recv_queue.front();
	this->recv_queue.pop();
	this->rcvmtx_queue.unlock();
	return data;
}
