#include<iostream>
#include<string>
#include<format>
#include<uWebSockets/App.h>

struct _UserData
{
	unsigned long long user_id;
	std::string user_name;
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

int main()
{
	_ProxyUserIDGenerator _proxy_user_id_generator;
	std::string pattern = "ws:127.0.0.1/";
	int port_to_listen = 9999;
	uWS::TemplatedApp templated_uws_app = uWS::App();
	uWS::App ws_app = templated_uws_app.ws<_UserData>(
		pattern,
		{
			.open = [&](auto* ws)
				{
				// что делать при подключении пользователя
				_UserData* user_data = (_UserData*)ws->getUserData();
				user_data->user_id = _proxy_user_id_generator.MakeNewID();
				},
			.message = [](auto* ws, std::string_view message, uWS::OpCode pr_code)
				{
					// что делать при получении сообщения

				}
		});
	ws_app.listen(port_to_listen, 
		[&](auto* token) 
		{
			std::cout << std::format("start listen on port {}",port_to_listen); 
		});

	return 0;
}