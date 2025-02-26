#include <iostream>
#include <asio.hpp>
#include "utils/time_utils.hpp"

static void wait_and_print(asio::steady_timer& timer, const std::string& timer_name) {
    try {
        timer.wait();  // 同步等待
        std::cout << "[" << utils::get_current_time() << "] "
                  << "Timer " << timer_name << " completed normally!" << std::endl;
    } catch (const std::system_error& e) {
        std::cout << "[" << utils::get_current_time() << "] "
                  << "Timer " << timer_name << " was cancelled. Error: " 
                  << e.what() << std::endl;
    }
}

int main() {
    asio::io_context io;
    asio::steady_timer t1(io, asio::chrono::seconds(1));
    asio::steady_timer t2(io, asio::chrono::seconds(2));
    asio::steady_timer t3(io, asio::chrono::seconds(3));

    std::cout << "[" << utils::get_current_time() << "] "
              << "****************start****************" << std::endl;

    // 同步等待，按顺序执行
    wait_and_print(t1, "1");
    wait_and_print(t2, "2");
    wait_and_print(t3, "3");

    std::cout << "[" << utils::get_current_time() << "] "
              << "****************end****************" << std::endl;
    return 0;
} 