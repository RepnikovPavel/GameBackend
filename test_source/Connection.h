#pragma once
#include <string>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>

class ConnectionsManagerInTheCurrentThread final
{
public:		ConnectionsManagerInTheCurrentThread();
public:		~ConnectionsManagerInTheCurrentThread();
public:		void StartListenServer();

public:		void PushDrawRequest(const std::vector<double>& X,const std::vector<double>& Y);
public:		void WaitUntilAllRenderingRequestsAreSent();

private:	std::atomic<int>			_connections_count;
private:	bool						_need_stop_server;
private:	bool						_user_try_make_connection;
private:	bool						_is_it_possible_to_add_to_the_queue;
private:	bool						_everything_that_needed_to_be_written_to_the_socket_is_written;

private:	std::queue<std::string>		_queue_for_rendering;
private:	std::mutex					_queue_mutex;
};