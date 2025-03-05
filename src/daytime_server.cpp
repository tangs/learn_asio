#include <iostream>
#include <string>
#include <asio.hpp>
#include "utils/time_utils.h"

using asio::ip::tcp;

int main() {
    try {
        asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1313));

        std::cout << "[" << utils::get_current_time() << "] "
                  << "Daytime server started on port 1313" << std::endl;

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::string message = utils::get_current_time();

            std::error_code ignored_error;
            asio::write(socket, asio::buffer(message), ignored_error);
        }
    } catch (std::exception& e) {
        std::cerr << "[" << utils::get_current_time() << "] "
                  << "Exception: " << e.what() << std::endl;
    }

    return 0;
} 