#include<iostream>
#include<string>
#include<format>
#include<uWebSockets/App.h>

struct _UserData
{
	unsigned long long user_id;
	std::string user_name = "anonymous user";
	// user_name is "renderer" or "client"
};

class _ProxyUserIDGenerator
{
public:
	unsigned long long MakeNewID()
	{
		_count++;
		return _count;
	}
private:
	unsigned long long _count = 0;
};

class _MessageHandler
{
public:
	void close_socket_if(auto* ws,std::string_view& message)
	{
		if (message=="closedown")
		{
			ws->close();
		}
	}
	bool subscribe_a_user_if(auto* ws, std::string user_name, unsigned long long user_id)
	{
		if (user_name == "renderer" || user_name == "client")
		{
			ws->subscribe(user_name+std::to_string(user_id));
			return true;
		}
		else
		{
			return false;
		}
	}
};




int main()
{
	_ProxyUserIDGenerator _proxy_user_id_generator;
	_MessageHandler _message_handler;
	std::string pattern = "ws://127.0.0.1/";
	int port_to_listen = 9999;
	uWS::TemplatedApp templated_uws_app = uWS::App();
	uWS::App ws_app = templated_uws_app.ws<_UserData>(
		pattern,
		{
			.open = [&](auto* ws)
				{
					// when client is connected
						//get user data from opened web socket set id for user
					_UserData* user_data = (_UserData*)ws->getUserData();
					user_data->user_id = _proxy_user_id_generator.MakeNewID();
						//subscibe to topic 
					_message_handler.subscribe_a_user_if(ws, user_data->user_name, user_data->user_id);
				
				},
			.message = [&](auto* ws, std::string_view message, uWS::OpCode op_code)
				{
					// when message came from opened ws
					_message_handler.close_socket_if(ws, message);
					_UserData* user_data = (_UserData*)ws->getUserData();
					std::cout << std::format("user id {}, user name {}, message {}", user_data->user_id, user_data->user_name, message)<<std::endl;
					ws->publish(user_data->user_name +  std::to_string(user_data->user_id),
						std::format("dear {} we have accepted your message \"{}\"", user_data->user_name, message),
						op_code, true);
					std::cout << std::format("ws->publish({})", user_data->user_name) << std::endl;

				}
		});
	ws_app.listen(port_to_listen, 
		[&](auto* token) 
		{
			if (token)
			{
				std::cout << std::format("token exists. start listen on port {}", port_to_listen)<<std::endl;
			}
			else
			{
				std::cout << "token is nullptr"<<std::endl;
			}
			
		}).run();
	
	return 0;
}