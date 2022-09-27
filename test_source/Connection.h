#pragma once
#include <string>
#include <queue>
#include <atomic>

class ConnectionsManagerInTheCurrentThread final
{
public:		ConnectionsManagerInTheCurrentThread();
public:		~ConnectionsManagerInTheCurrentThread();

private:	std::atomic<int>	_connections_count;
private:	bool	_need_stop_server;
private:	bool	_user_try_make_connection;
private:	std::queue<std::string> _queue_for_rendering;
};