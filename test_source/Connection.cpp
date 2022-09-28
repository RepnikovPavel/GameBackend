#include "Connection.h"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
//#include <cstdlib>
//#include <functional>
#include <iostream>
#include <thread>
#include <sstream>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace server_impl
{
    //void do_session(tcp::socket socket, std::queue<char>& queue_for_rendering, bool& need_stop_server)
    void do_session(tcp::socket socket,
        bool* everything_that_needed_to_be_written_to_the_socket_is_written,
        std::atomic<int>* connections_count_ptr, bool* need_stop_server, 
        std::queue<std::string>* queue_for_rendering_ptr, std::mutex* queue_mutex)
    {   
        std::cout << "in do_session" << std::endl;
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
            while((*need_stop_server == false)||(!queue_for_rendering_ptr->empty()))
            {
                if (!queue_for_rendering_ptr->empty())
                {   
                    *everything_that_needed_to_be_written_to_the_socket_is_written = false;
                    queue_mutex->lock();
                    std::string task_description = queue_for_rendering_ptr->front();
                    queue_for_rendering_ptr->pop();
                    queue_mutex->unlock();
                    boost::asio::const_buffer buffer(task_description.c_str(), task_description.size());
                    std::cout << "before ws.write(buffer)" << std::endl;
                    ws.write(buffer);
                    if (queue_for_rendering_ptr->empty())
                    {
                        *everything_that_needed_to_be_written_to_the_socket_is_written = true;
                    }
                    
                    std::cout << "after ws.write(buffer)" << std::endl;
                    
                }
            }
            std::cout << "return from do_session"<<std::endl;
            /*socket.close();*/ // do socket dead correct by myself
            (*connections_count_ptr)--;
            std::cout << "connections_count_ptr="<<*(connections_count_ptr) << std::endl;
        }
        catch (beast::system_error const& se)
        {
            // This indicates that the session was closed
            if (se.code() != websocket::error::closed)
                std::cout << "Error: " << se.code().message() << std::endl;
            
            (*connections_count_ptr)--;
        }
        catch (std::exception const& e)
        {
            std::cout << "Error: " << e.what() << std::endl;
            
            (*connections_count_ptr)--;
        }
    }
    /*void run_serv(std::queue<char>& queue_for_rendering,bool& need_stop_server)*/
    void run_serv(
        bool* everything_that_needed_to_be_written_to_the_socket_is_written,
        bool* user_try_make_connection,std::atomic<int>* connections_count_ptr,bool* need_stop_server, 
        std::queue<std::string>* queue_for_rendering_ptr, std::mutex* queue_mutex)
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
            while (*need_stop_server==false || *user_try_make_connection==false || !queue_for_rendering_ptr->empty() || (*everything_that_needed_to_be_written_to_the_socket_is_written == false))
            {
                if (need_new_connection)
                {
                    // This will receive the new connection
                    tcp::socket socket{ ioc };

                    // Block until we get a connection
                    (*connections_count_ptr)++;
                    need_new_connection = false;
                    std::cout << "acceptor.accept(socket)"<<std::endl;
                    acceptor.accept(socket);
                    std::cout << "user_try_make_connection = true;"<<std::endl;
                    *user_try_make_connection = true;

                    // Launch the session, transferring ownership of the socket
                    /*std::thread(&do_session,std::move(socket),queue_for_rendering, need_stop_server).detach();*/
                    std::thread(&do_session, std::move(socket),
                        everything_that_needed_to_be_written_to_the_socket_is_written,
                        connections_count_ptr, need_stop_server, queue_for_rendering_ptr, queue_mutex).detach();
                }
            }
            std::cout << "return from run_serv" << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "Error: " << e.what() << std::endl;
        }

    }

}

ConnectionsManagerInTheCurrentThread::ConnectionsManagerInTheCurrentThread()
{   
    _is_it_possible_to_add_to_the_queue = true;
    _need_stop_server = false; // need be in start server
    _user_try_make_connection = false;// need be in start server
    _connections_count = 0;// need be in start server
    _everything_that_needed_to_be_written_to_the_socket_is_written = true;// need be in start server
}
void ConnectionsManagerInTheCurrentThread::StartListenServer()
{
    std::thread(&server_impl::run_serv,&_everything_that_needed_to_be_written_to_the_socket_is_written, &_user_try_make_connection, &_connections_count, &_need_stop_server, &_queue_for_rendering, &_queue_mutex).detach();
}

void ConnectionsManagerInTheCurrentThread::WaitUntilAllRenderingRequestsAreSent()
{
    std::cout << "_need_stop_server = true" << std::endl;
    _need_stop_server = true;
    while (!((_connections_count == 0) && (_user_try_make_connection == true) && (_everything_that_needed_to_be_written_to_the_socket_is_written==true)))
    {
    }
    std::cout << "all conections closed" << std::endl;
}

ConnectionsManagerInTheCurrentThread::~ConnectionsManagerInTheCurrentThread()
{   

}

void ConnectionsManagerInTheCurrentThread::PushDrawRequest(const std::vector<double>& X, const std::vector<double>& Y)
{

    try 
    {
        if (X.size() != Y.size()) { throw std::logic_error("X.size()!=Y.size() in PushDrawRequest function"); }
        std::string _X;
        std::string _Y;

        for (size_t i = 0; i < X.size(); i++)
        {
            _X += std::to_string(X[i]);
            _X += ",";
        }
        for (size_t i = 0; i < Y.size(); i++)
        {
            _Y += std::to_string(Y[i]);
            _Y += ",";
        }

        std::stringstream request;
        request << "start_plot_request;2Dplot;" << "X{" << _X << "};" << "Y{" << _Y << "};end_plot_request;";
        
        _queue_mutex.lock();
        _queue_for_rendering.push(request.str());
        _everything_that_needed_to_be_written_to_the_socket_is_written = false;
        std::cout << "new plot request" << std::endl;
        _queue_mutex.unlock();

    }
    catch (const std::bad_alloc& ex)
    {
        std::cout << "allocation failed in PushDrawRequest function:\t" << ex.what() << std::endl;
        
    }
    catch (const std::logic_error& ex)
    {
        std::cout << ex.what() << std::endl;
        
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        
    }
    catch (...)
    {
        std::cout << "unknown exeption in PushDrawRequest function" << std::endl;
        
    }

}