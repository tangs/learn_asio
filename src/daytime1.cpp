#include <array>
#include <iostream>
#include <string>
#include <thread>
#include <asio.hpp>

#include "utils/time_utils.h"

class async_tcp_client {
public:
    async_tcp_client(asio::io_context& io_context, const std::string& host) 
        : socket_(io_context), 
          resolver_(io_context) {
        // Resolve the host and port
        const auto endpoints = resolver_.resolve(host, "1313");
        
        // Connect asynchronously
        asio::async_connect(socket_, endpoints, 
            [this](const std::error_code& error, const asio::ip::tcp::endpoint& /*endpoint*/) {
                if (!error) {
                    std::cout << "[" << utils::get_current_time() << "] "
                              << "Connected to server" << std::endl;
                    start_read();
                } else {
                    std::cerr << "[" << utils::get_current_time() << "] "
                              << "Connection error: " << error.message() << std::endl;
                }
            }
        );
    }

    void send_message(const std::string& message) {
        asio::async_write(socket_, 
            asio::buffer(message), 
            [this, message](const std::error_code& error, std::size_t bytes_transferred) {
                if (!error) {
                    std::cout << "[" << utils::get_current_time() << "] "
                              << "Sent: " << message 
                              << " (" << bytes_transferred << " bytes)" << std::endl;
                } else {
                    std::cerr << "[" << utils::get_current_time() << "] "
                              << "Send error: " << error.message() << std::endl;
                }
            }
        );
    }

private:
    void start_read() {
        socket_.async_read_some(
            asio::buffer(read_buffer_),
            [this](const std::error_code& error, std::size_t bytes_transferred) {
                if (!error) {
                    const std::string received(read_buffer_.data(), bytes_transferred);
                    std::cout << "[" << utils::get_current_time() << "] "
                              << "Received: " << received << std::endl;
                    
                    // Continue reading
                    start_read();
                } else if (error == asio::error::eof) {
                    std::cout << "[" << utils::get_current_time() << "] "
                              << "Connection closed by server" << std::endl;
                    std::exit(0);
                } else {
                    std::cerr << "[" << utils::get_current_time() << "] "
                              << "Read error: " << error.message() << std::endl;
                    std::exit(1);
                }
            }
        );
    }

    asio::ip::tcp::socket socket_;
    asio::ip::tcp::resolver resolver_;
    std::array<char, 1024> read_buffer_{};
};

int main(const int argc, const char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "[" << utils::get_current_time() << "] "
                      << "Usage: client <host>" << std::endl;
            return 1;
        }

        asio::io_context io_context;
        async_tcp_client client(io_context, argv[1]);

        // Run IO context in a separate thread
        std::thread io_thread([&io_context]() {
            io_context.run();
        });

        // Main thread for keyboard input
        std::string input;
        while (true) {
            std::getline(std::cin, input);
            
            // Exit condition
            // if (input == "exit" || input == "quit") {
            if (input == "exit") {
                break;
            }

            // Send input to server
            client.send_message(input + "\n");
        }

        // Stop the IO context
        io_context.stop();
        io_thread.join();

    } catch (std::exception& e) {
        std::cerr << "[" << utils::get_current_time() << "] "
                  << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
