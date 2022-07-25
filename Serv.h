#pragma once
#include <vector>
#include <thread>
class Serv
{
		
	std::vector<std::thread*> Threads;
	
	template<class USER_DB_CONNECTION_INFO>
	class DBConnection;

	template<class USER_WS_CONNECTION_INFO>
	class WSConnection;

	class WSApplication;

	void StartListen();
	void StopListen();

};

 