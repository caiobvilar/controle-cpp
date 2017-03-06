#include "communication.hpp"

///////////////////METHODS///////////////////////////////

Communication::Communication(std::string ip, int portno)
{
	this->srv_ip = ip.c_str();
	this->port = portno;
	this->fdread = StartConn();
	//this->fdwrite = StartConn();
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
	if(this->resolveHostname(this->srv_ip.c_str(),&(address.sin_addr)) != 0)
	{
		inet_pton(PF_INET,this->srv_ip.c_str(), &(address.sin_addr));
	}
	int sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if(sock_fd < 0)
	{
		std::cerr << "[ERROR]: Socket() failed." << std::endl;
		return -1;
	}
	if(connect(sock_fd,(struct sockaddr*)&address,sizeof(address))!= 0)
	{
		std::cerr << "[ERROR]: Connect() failed." << std::endl;
		return -1;
	}
	return sock_fd;
}

int Communication::EndConn(int fd)
{
	int res;
	if((res =	close(fd)) < 0)
	{
		std::cerr << "[ERROR]: Close() failed | ERRNO: " << strerror(errno) << std::endl;
	}
	return res;
}

int Communication::resolveHostname(const char *hostname,struct in_addr *addr)
{
	struct addrinfo *res;

	int result = getaddrinfo(hostname, NULL, NULL, &res);
	if(result == 0)
	{
		memcpy(addr,&((struct sockaddr_in *)res->ai_addr)->sin_addr, sizeof(struct in_addr));
		freeaddrinfo(res);
	}
	return result;
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
	int res; //error return of functions
	std::string snd_value;
	std::chrono::duration<double,std::milli> dur;
	std::chrono::duration<double,std::milli> crit_time(100);
	auto start = std::chrono::high_resolution_clock::now();

	this->sndmtx_queue.lock();
	while(!send_queue.empty() && !this->isstopped())
	{
		snd_value = send_queue.front();
		this->send_queue.pop();
		auto end = std::chrono::high_resolution_clock::now();
		dur = end - start;
		if(dur < crit_time)
		{
			std::this_thread::sleep_for(crit_time - dur);
		}
		res = writeDA(this->fdwrite,this->DACChannel,std::stof(snd_value));
		if(res < 0)
		{
			std::cerr << "[ERROR]: Send() failed | ERRNO: " << strerror(errno) << std::endl;
			this->setstopthreads(true); //faiou 

		}
		start = std::chrono::high_resolution_clock::now();
	}
	this->sndmtx_queue.unlock();
}

void Communication::RunRcv()
{
	int res; //function call return for error checking
	std::chrono::duration<double,std::milli> startup(90);
	this->rcvmtx_queue.lock();
	while((this->recv_queue.size() < 100) && (!this->isstopped()))
	{
		std::this_thread::sleep_for(startup);
		res = readAD(this->ADCChannel,this->fdread);
		if(res < 0)
		{
			std::cerr << "[ERROR]: readAD() failed | ERRNO: " << strerror(errno) << std::endl; 
			this->setstopthreads(true); //faiou 
		}
		this->recv_queue.push(this->recvstring);
	}
	this->rcvmtx_queue.unlock();
}

void Communication::insert_snd_queue(std::string data)
{
	this->sndmtx_queue.lock();
	if(this->recv_queue.size() < 100)
	{
		this->send_queue.push(data);
	}
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
