#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>

#include <boost/config.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <boost/smart_ptr.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <boost/asio/signal_set.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <vector>

#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "PlotLib.h"
#include "Listener.h"
#include "SharedState.h"

namespace server_impl
{
    namespace beast = boost::beast;         // from <boost/beast.hpp>
    namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
    namespace net = boost::asio;            // from <boost/asio.hpp>
    using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

   
    void run_serv()
    {
        auto address = net::ip::make_address("127.0.0.1");
        auto port = static_cast<unsigned short>(std::atoi("8080"));
        auto doc_root = ".";
        auto const threads = std::max<int>(1, std::atoi("1"));

        // The io_context is required for all I/O
        net::io_context ioc;

        // Create and launch a listening port
        boost::make_shared<listener>(
            ioc,
            tcp::endpoint{ address, port },
            boost::make_shared<shared_state>(doc_root))->run();

        // Capture SIGINT and SIGTERM to perform a clean shutdown
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait(
            [&ioc](boost::system::error_code const&, int)
            {
                // Stop the io_context. This will cause run()
                // to return immediately, eventually destroying the
                // io_context and any remaining handlers in it.
                ioc.stop();
            });

        // Run the I/O service on the requested number of threads
        std::vector<std::thread> v;
        v.reserve(threads - 1);
        for (auto i = threads - 1; i > 0; --i)
            v.emplace_back(
                [&ioc]
                {
                    ioc.run();
                });
        ioc.run();

        // (If we get here, it means we got a SIGINT or SIGTERM)

        // Block until all the threads exit
        for (auto& t : v)
            t.join();
    }

}

void plt::Manager::Init()
{
	std::cout << "plt::Manager::Init works" << std::endl;
    serv_thread = std::thread(&server_impl::run_serv);
}
void plt::Manager::StopCurrentThread()
{
    std::string meaningless_string;
    std::cout << "master thread is locked. type some symbol or string to continue master thread:" << std::endl;
    std::cin >> meaningless_string;
}

