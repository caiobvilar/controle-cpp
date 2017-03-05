#ifndef _CONTROLLER_HPP
#define _CONTROLLER_HPP
//////////////INCLUDES///////////////////
#include <iostream>
#include <sstream>
#include <thread>
#include <iomanip>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <string>
//include dos modulos
#include "malha.hpp"
#include "communication.hpp"
#include "ui.hpp"
///////////////////////////////////////

class Controller
{
	private:
	bool working;	
	public:
	Controller();
	~Controller();

};

#endif // _CONTROLLER_HPP
