#include <iostream>
#include <asio.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string get_current_time() {
    const auto now = std::chrono::system_clock::now();
    const auto now_c = std::chrono::system_clock::to_time_t(now);
    const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
    return ss.str();
}

void print(const std::error_code& e) {
    if (e) {
        std::cout << "[" << get_current_time() << "] "
                  << "Timer 1 was cancelled. Error: " << e.message() << std::endl;
        return;
    }
    std::cout << "[" << get_current_time() << "] "
              << "Timer 1 completed normally!" << std::endl;
}

void print1(const std::error_code& e) {
    std::cout << "[" << get_current_time() << "] "
              << "Timer 2 completed with error code: " << e.message() << std::endl;
}

int main() {
    asio::io_context io;
    asio::steady_timer t1(io, asio::chrono::seconds(5));
    asio::steady_timer t2(io, asio::chrono::seconds(3));
    asio::steady_timer t3(io, asio::chrono::seconds(2));

    std::cout << "[" << get_current_time() << "] "
                      << "start..." << std::endl;

    t1.async_wait(&print);
    t2.async_wait(&print1);
    t3.async_wait([&t1] (const std::error_code& e) {
        std::cout << "[" << get_current_time() << "] "
                  << "Timer 3 completed, cancelling t1..." << std::endl;
        t1.cancel();
    });

    io.run();
    return 0;
}