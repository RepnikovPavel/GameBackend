#pragma once
#include <iostream>
#include <thread>
#include <vector>
namespace plt 
{
	class Manager
	{
	public:
		void Init();
		void StopCurrentThread();
		template<typename _Elem>
		void Plot(const std::vector<_Elem>& x, const std::vector<_Elem>& y)
		{
		};
	private:
		std::thread serv_thread;
	};




}