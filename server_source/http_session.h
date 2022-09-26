#pragma once

#include <boost/optional.hpp>
#include <boost/smart_ptr.hpp>
#include <cstdlib>
#include <memory>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "SharedState.h"

namespace beast = boost::beast;                 // from <boost/beast.hpp>
namespace http = boost::beast::http;            // from <boost/beast/http.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>

namespace net = boost::asio;                    // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>


/** Represents an established HTTP connection
*/
class http_session : public boost::enable_shared_from_this<http_session>
{
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    boost::shared_ptr<shared_state> state_;

    // The parser is stored in an optional container so we can
    // construct it from scratch it at the beginning of each new message.
    boost::optional<http::request_parser<http::string_body>> parser_;

    struct send_lambda;

    void fail(beast::error_code ec, char const* what);
    void do_read();
    void on_read(beast::error_code ec, std::size_t);
    void on_write(beast::error_code ec, std::size_t, bool close);

public:
    http_session(
        tcp::socket&& socket,
        boost::shared_ptr<shared_state> const& state);

    void run();
};