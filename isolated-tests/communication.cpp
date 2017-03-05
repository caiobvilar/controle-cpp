#include "communication.hpp"

///////////////////METHODS///////////////////////////////

Communication::Communication(std::string ip, int portno)
{
	this->connected = false;
	this->srv_ip = ip.c_str();
	this->port = portno;
	this->fdread = StartConn(this->port);
	//this->fdwrite = StartConn(this->port);
	this->Start();
}

Communication::~Communication()
{
	//Ends connection with the server
	this->EndConn(this->fdread);
	//this->EndConn(this->fdwrite);
	this->Stop();
}

int Communication::StartConn()
{
	struct sockaddr_in address;
	memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(this->port);
	if(this->resolveHostname(this->server,&(address.sin_addr)) != 0)
	{
		inet_pton(PF_INET,this->server, &(address.sin_addr));
	}
	int sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if(sock_fd < 0)
	{
		std::cerr << "[ERROR]: Socket() failed." << std::endl;
		return NULL;
	}
	if(connect((struct sockaddr*)&address,sizeof(address))!= 0)
	{
		std::cerr << "[ERROR]: Connect() failed." << std::endl;
		return NULL;
	}
	return sock_fd;
}

int Communication::EndConn(int fd)
{
	//close sockets
	close(fd);
	//	close(this->fdwrite);
}

bool Communication::isstopped()
{
	bool ret;
	this->stopmtx_threads.lock();
	ret = this->stopthreads;
	this->stopmtx_threads.unlock();
	return ret;
}

void Communication::setstopthreads(bool setting)
{
	this->stopmtx_threads.lock();
	this->stopthreads = setting;
	this->stopmtx_threads.unlock();
}

void Communication::Start()
{
	if(!this->isstopped())
	{
		this->recv_thread = std::thread(&Communication::RunRcv,this);	//Start receiving queue management thread first
		//this->send_thread = std::thread(&Communication::RunSnd,this);
	}
	else
	{
		this->setstopthreads(false);
		this->recv_thread = std::thread(&Communication::RunRcv,this);	//Start receiving queue management thread first
		//this->send_thread = std::thread(&Communication::RunSnd,this);
	}

}

void Communication::Stop()
{
	if(this->isstopped()) //stop threads if an outer method told to stop
	{
		this->recv_thread.join();
	//this->send_thread.join();
	}
	this->setstopthreads(true); //stops threads forcefully
	this->recv_thread.join();
	//this->send_thread.join();
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
	this->rcvmtx_queue.lock();
	this->recv_queue.push(this->recvstring);
	this->rcvmtx_queue.unlock();
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
