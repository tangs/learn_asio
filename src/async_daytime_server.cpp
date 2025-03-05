#include <iostream>
#include <string>
#include <asio.hpp>
#include "utils/time_utils.h"

class tcp_connection
    : public std::enable_shared_from_this<tcp_connection> {
public:
    using pointer = std::shared_ptr<tcp_connection>;

    static auto create(asio::io_context& io_context) {
        return std::make_shared<tcp_connection>(tcp_connection(io_context));
    }

    asio::ip::tcp::socket& socket() {
        return socket_;
    }

    void start() {
        // 获取客户端的 endpoint
        const auto remote_endpoint = socket_.remote_endpoint();
        const auto client_address = remote_endpoint.address().to_string();
        const auto client_port = remote_endpoint.port();

        std::cout << "[" << utils::get_current_time() << "] "
                  << "Client connected from " << client_address 
                  << ":" << client_port << std::endl;

        message_ = utils::get_current_time();
        async_write(socket_, asio::buffer(message_), 
            [client_address, client_port](const auto& error, const size_t bytes_transferred) {
                std::cout << "[" << utils::get_current_time() << "] "
                      << "Write to client " << client_address << ":" << client_port
                      << ". err: " << error.message()
                      << ", bytes transferred:" << bytes_transferred << std::endl;
            });
    }
private:
    explicit tcp_connection(asio::io_context& io_context)
        : socket_(io_context) {
    }

    asio::ip::tcp::socket socket_;
    std::string message_;
};

class tcp_server
    : public std::enable_shared_from_this<tcp_server> {
public:
    explicit tcp_server(asio::io_context& io_context)
        : io_context_(io_context)
        , acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 1313)) {
        std::cout << "[" << utils::get_current_time() << "] "
                  << "Server started on port 1313" << std::endl;
    }

    void start_accept() {
        const auto new_connection = tcp_connection::create(io_context_);
        acceptor_.async_accept(new_connection->socket(), [new_connection,
            tcp_server_wk = weak_from_this()] (const auto& error) {
                if (!error) {
                    new_connection->start();
                } else {
                    std::cout << "[" << utils::get_current_time() << "] "
                              << "Accept error: " << error.message() << std::endl;
                }

                const auto tcp_server = tcp_server_wk.lock();
                if (!tcp_server) return;
                tcp_server->start_accept();
        });
    }
private:
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
};

int main() {
    try {
        asio::io_context io_context;
        const auto server = std::make_shared<tcp_server>(io_context);
        server->start_accept();
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "[" << utils::get_current_time() << "] "
                  << "Exception: " << e.what() << std::endl;
    }

    return 0;
}