//server
#include <iostream>
#include <string>
#include <format>
#include <vector>
#include <thread>


#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

//	Data Type Ranges
//		Type Name			Bytes	Range of Values
//		int					4		-2,147,483,648 to 2,147,483,647
//		unsigned int		4		0 to 4,294,967,295
//		short				2		-32,768 to 32,767
//		unsigned short		2		0 to 65,535
//		long				4		-2,147,483,648 to 2,147,483,647
//		unsinged long		4		0 to 4,294,967,295
//		long long			8		-9,223,372,036,854,775,808 to 9,223,372,036,854,775,807
//		unsigned long long	8		0 to 18,446,744,073,709,551,615
//		float				4
//		double				8
//		char				1		-128 to 127
//		usigned char		1		0 to 255
//		bool				1		false or true

namespace beast = boost::beast;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class ThreadTracker
{
public:
	unsigned int GetNewThreadID()
	{
		return num_of_thread++;
	}
private:
	std::atomic<unsigned int> num_of_thread = 0;
};

int main(int argc, char** argv)
{
	SetConsoleOutputCP(CP_UTF8);

	const auto address = boost::asio::ip::make_address("127.0.0.1");
	const auto port = static_cast<unsigned short>(std::atoi("8083"));

	net::io_context ioc{ 1 };
	tcp::acceptor acceptor{ ioc,{address,port} };

	ThreadTracker thread_tracker;
	while (true)
	{
		tcp::socket socket{ ioc };
		acceptor.accept(socket);
		std::cout << "socket accepted" << std::endl;

		std::thread new_thread(
			[q{ std::move(socket) }, &thread_tracker]()
			{
				unsigned int ThreadID = thread_tracker.GetNewThreadID();
				beast::websocket::stream<tcp::socket> ws{ std::move(const_cast<tcp::socket&>(q)) };
				ws.accept();
				while (true)
				{
					try {
						beast::flat_buffer buffer;
						ws.read(buffer);
						auto message_from_socket = beast::buffers_to_string(buffer.cdata());
						std::cout << std::format("message from socket {} :{}", ThreadID, message_from_socket) << std::endl;
					}
					catch (const beast::system_error& se)
					{
						if (se.code() != beast::websocket::error::closed)
						{
							std::cout << se.code().message() << std::endl;
							break;
						}
					}
				}
			}
		);
		new_thread.detach();
	}

	return EXIT_SUCCESS;
}