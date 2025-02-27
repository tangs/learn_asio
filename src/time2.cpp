#include <iostream>
#include <asio.hpp>
#include "utils/time_utils.h"

static void print(const std::error_code& e) {
    if (e) {
        std::cout << "[" << utils::get_current_time() << "] "
                  << "Timer 1 was cancelled. Error: " << e.message() << std::endl;
        return;
    }
    std::cout << "[" << utils::get_current_time() << "] "
              << "Timer 1 completed normally!" << std::endl;
}

void print1(const std::error_code& e) {
    std::cout << "[" << utils::get_current_time() << "] "
              << "Timer 2 completed with error code: " << e.message() << std::endl;
}

int main() {
    asio::io_context io;
    asio::steady_timer t1(io, asio::chrono::seconds(1));
    asio::steady_timer t2(io, asio::chrono::seconds(3));
    asio::steady_timer t3(io, asio::chrono::seconds(2));

    std::cout << "[" << utils::get_current_time() << "] "
              << "****************start****************" << std::endl;

    t1.async_wait(&print);
    t2.async_wait(&print1);
    t3.async_wait([&t1] (const std::error_code& e) {
        std::cout << "[" << utils::get_current_time() << "] "
                  << "Timer 3 completed, cancelling t1..." << std::endl;
        t1.cancel();
    });

    io.run();
    std::cout << "[" << utils::get_current_time() << "] "
              << "****************end****************" << std::endl;
    return 0;
} 