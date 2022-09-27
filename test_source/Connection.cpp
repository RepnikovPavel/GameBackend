#include "Connection.h"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
//#include <cstdlib>
//#include <functional>
#include <iostream>
#include <thread>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace server_impl
{
    //void do_session(tcp::socket socket, std::queue<char>& queue_for_rendering, bool& need_stop_server)
    void do_session(tcp::socket socket, std::atomic<int>* connections_count_ptr, bool* need_stop_server, std::queue<std::string>* queue_for_rendering_ptr)
    {   
        try
        {
            // Construct the stream by moving in the socket
            websocket::stream<tcp::socket> ws{ std::move(socket) };

            // Set a decorator to change the Server of the handshake
            ws.set_option(websocket::stream_base::decorator(
                [](websocket::response_type& res)
                {
                    res.set(http::field::server,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-sync");
                }));

            // Accept the websocket handshake
            ws.accept();
            while((*need_stop_server == false)||(queue_for_rendering_ptr->size() > 0))
            {
                if (queue_for_rendering_ptr->size() > 0)
                {
                    std::string task_description = queue_for_rendering_ptr->front();
                    queue_for_rendering_ptr->pop();
                    boost::asio::const_buffer buffer(task_description.c_str(), task_description.size());
                    ws.write(buffer);
                }
            }
            /*socket.close();*/ // do socket dead correct by myself
            (*connections_count_ptr)--;
        }
        catch (beast::system_error const& se)
        {
            // This indicates that the session was closed
            if (se.code() != websocket::error::closed)
                std::cerr << "Error: " << se.code().message() << std::endl;

            (*connections_count_ptr)--;
        }
        catch (std::exception const& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            (*connections_count_ptr)--;
        }
    }
    /*void run_serv(std::queue<char>& queue_for_rendering,bool& need_stop_server)*/
    void run_serv(bool* user_try_make_connection,std::atomic<int>* connections_count_ptr,bool* need_stop_server, std::queue<std::string>* queue_for_rendering_ptr)
    {
        try
        {
            auto const address = net::ip::make_address("127.0.0.1");
            auto const port = static_cast<unsigned short>(std::atoi("8080"));

            // The io_context is required for all I/O
            net::io_context ioc{ 1 };

            // The acceptor receives incoming connections
            tcp::acceptor acceptor{ ioc, {address, port} };
            

            bool need_new_connection = true;
            while (*need_stop_server==false || *user_try_make_connection==false)
            {
                if (need_new_connection)
                {
                    // This will receive the new connection
                    tcp::socket socket{ ioc };

                    // Block until we get a connection
                    (*connections_count_ptr)++;
                    need_new_connection = false;
                    std::cout << "we are waiting for you to connect from the browser...";
                    acceptor.accept(socket);
                    *user_try_make_connection = true;

                    // Launch the session, transferring ownership of the socket
                    /*std::thread(&do_session,std::move(socket),queue_for_rendering, need_stop_server).detach();*/
                    std::thread(&do_session, std::move(socket),connections_count_ptr, need_stop_server, queue_for_rendering_ptr).detach();
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }

    }

}

ConnectionsManagerInTheCurrentThread::ConnectionsManagerInTheCurrentThread()
{   
    _need_stop_server = false;
    _user_try_make_connection = false;
    _connections_count = 0;
    std::thread(&server_impl::run_serv,&_user_try_make_connection, &_connections_count,&_need_stop_server, &_queue_for_rendering).detach();
}
ConnectionsManagerInTheCurrentThread::~ConnectionsManagerInTheCurrentThread()
{   
    std::cout << "trying closing all conections..." << std::endl;
    _need_stop_server = true;
    while ( !((_connections_count == 0)&&(_user_try_make_connection == true)))
    {
    }
    std::cout << "all conections closed" << std::endl;
}
