#pragma once
#include <iostream>
#include <thread>
namespace plt 
{
	class Manager
	{
	public:
		void Init();
		void StopMasterThread();
	private:
		std::thread serv_thread;
	};




}