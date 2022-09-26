#pragma once
#include <boost/smart_ptr.hpp>
#include <memory>
#include <string>

#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>


namespace beast = boost::beast;                 // from <boost/beast.hpp>
namespace http = boost::beast::http;            // from <boost/beast/http.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>

namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

// Forward declaration
class shared_state;

// Accepts incoming connections and launches the sessions
class listener : public boost::enable_shared_from_this<listener>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    boost::shared_ptr<shared_state> state_;

    void fail(beast::error_code ec, char const* what);
    void on_accept(beast::error_code ec, tcp::socket socket);

public:
    listener(
        net::io_context& ioc,
        tcp::endpoint endpoint,
        boost::shared_ptr<shared_state> const& state);

    // Start accepting incoming connections
    void run();
};