#include <array>
#include <iostream>
#include <asio.hpp>

#include "utils/time_utils.h"

int main(const int argc, const char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "[" << utils::get_current_time() << "] "
                      << "Usage: client <host>" << std::endl;
            return 1;
        }

        std::cout << "[" << utils::get_current_time() << "] "
                  << "socket: " << argv[1] << std::endl;

        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);

        const auto endpoints = resolver.resolve(argv[1], "1313");
        asio::ip::tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        while (true) {
            std::array<char, 128> buf{};
            std::error_code error;

            const auto len = socket.read_some(asio::buffer(buf), error);

            if (error == asio::error::eof) {
                break;
            }
            if (error) {
                throw std::system_error(error);
            }

            std::cout << "[" << utils::get_current_time() << "] "
                      << "read: " << buf.data() << std::endl;

            std::cout.write(buf.data(), static_cast<const std::streamsize>(len));
        }

    } catch (std::exception& e) {
        std::cerr << "[" << utils::get_current_time() << "] "
                      << e.what() << std::endl;
    }
    return 0;
}
