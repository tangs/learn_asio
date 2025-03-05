#include <iostream>
#include <string>
#include <asio.hpp>
#include "utils/time_utils.h"
#include <chrono>

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

        // Start async read
        start_read();

        // Send current time to client
        message_ = utils::get_current_time();
        async_write(socket_, asio::buffer(message_), 
            [self = weak_from_this(), client_address, client_port](const auto& error, const size_t bytes_transferred) {
                const auto connection = self.lock();
                if (!connection) {
                    std::cout << "[" << utils::get_current_time() << "] "
                             << "Connection is null." << std::endl;
                    return;
                }
                if (error) connection->need_clear = true;

                std::cout << "[" << utils::get_current_time() << "] "
                      << "Write to client " << client_address << ":" << client_port
                      << ". err: " << error.message()
                      << ", bytes transferred:" << bytes_transferred << std::endl;
            });
    }

private:
    void start_read() {
        // Clear the read buffer before each read
        read_buffer_.fill(0);

        // Start an asynchronous read operation
        socket_.async_read_some(
            asio::buffer(read_buffer_),
            [self = weak_from_this()](const auto& error, const size_t bytes_transferred) {
                const auto connection = self.lock();
                // const auto& connection = self;
                if (!connection) {
                    std::cout << "[" << utils::get_current_time() << "] "
                             << "Connection is null." << std::endl;
                    return;
                }

                if (!error) {
                    // Process the received data
                    const std::string received_data(connection->read_buffer_.data(), bytes_transferred);
                    std::cout << "[" << utils::get_current_time() << "] "
                              << "Received data: " << received_data << std::endl;
                    if (utils::trim(received_data) == "quit") {
                        connection->need_clear = true;
                    } else {
                        async_write(connection->socket_, asio::buffer(received_data),
                        [connection](const auto& error, const size_t bytes_transferred) {
                            if (error) connection->need_clear = true;

                            std::cout << "[" << utils::get_current_time() << "] "
                                  << ". err: " << error.message()
                                  << ", bytes transferred:" << bytes_transferred << std::endl;
                        });
                        // Continue reading
                        connection->start_read();
                    }
                } else if (error == asio::error::eof) {
                    // Connection closed cleanly by peer
                    std::cout << "[" << utils::get_current_time() << "] "
                              << "Connection closed by client" << std::endl;
                    connection->need_clear = true;
                } else {
                    // Handle other errors
                    std::cout << "[" << utils::get_current_time() << "] "
                              << "Read error: " << error.message() << std::endl;
                    connection->need_clear = true;
                }
            }
        );
    }

    explicit tcp_connection(asio::io_context& io_context)
        : socket_(io_context) {
    }

    asio::ip::tcp::socket socket_;
    std::string message_;
    std::array<char, 1024> read_buffer_{}; // Buffer for reading data
public:
    bool need_clear{};
};

class tcp_server
    : public std::enable_shared_from_this<tcp_server> {
public:
    explicit tcp_server(asio::io_context& io_context)
        : io_context_(io_context)
        , acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 1313))
        , update_timer_(io_context) {
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
                tcp_server->connections_.push_back(new_connection);
                tcp_server->start_accept();
        });
    }

    void start() {
        // Start the periodic update timer
        start_update_timer();
        start_accept();
    }

    void update() {
        auto& connections = connections_;
        for (int i = static_cast<int>(connections.size()) - 1; i >= 0; --i) {
            if (connections[i]->need_clear) {
                connections[i].swap(connections.back());
                connections.pop_back();
                std::cout << "[" << utils::get_current_time() << "] "
                              << "Remove connection: " << connections.size() << std::endl;
            }
        }
    }

private:
    void start_update_timer() {
        update_timer_.expires_after(std::chrono::milliseconds(10));
        update_timer_.async_wait([self_wk = weak_from_this()](const auto& error) {
            const auto self = self_wk.lock();
            if (!self) return;
            if (error) {
                std::cout << "[" << utils::get_current_time() << "] "
                          << "Update timer error: " << error.message() << std::endl;
                return;
            }

            // Call update method
            self->update();

            // Restart the timer
            self->start_update_timer();
        });
    }

    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<tcp_connection>> connections_;
    asio::steady_timer update_timer_; // Added timer member
};

int main() {
    try {
        asio::io_context io_context;
        const auto server = std::make_shared<tcp_server>(io_context);
        server->start();
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "[" << utils::get_current_time() << "] "
                  << "Exception: " << e.what() << std::endl;
    }

    return 0;
}