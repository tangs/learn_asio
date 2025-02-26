#include <iostream>
#include <asio.hpp>
#include <iomanip>
#include <sstream>

static std::string get_current_time() {
    const auto now = std::chrono::system_clock::now();
    const auto now_c = std::chrono::system_clock::to_time_t(now);
    const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
    return ss.str();
}

static void wait_and_print(asio::steady_timer& timer, const std::string& timer_name) {
    try {
        timer.wait();  // 同步等待
        std::cout << "[" << get_current_time() << "] "
                  << "Timer " << timer_name << " completed normally!" << std::endl;
    } catch (const std::system_error& e) {
        std::cout << "[" << get_current_time() << "] "
                  << "Timer " << timer_name << " was cancelled. Error: " 
                  << e.what() << std::endl;
    }
}

int main() {
    asio::io_context io;
    asio::steady_timer t1(io, asio::chrono::seconds(1));
    asio::steady_timer t2(io, asio::chrono::seconds(2));
    asio::steady_timer t3(io, asio::chrono::seconds(3));

    std::cout << "[" << get_current_time() << "] "
              << "****************start****************" << std::endl;

    // 同步等待，按顺序执行
    wait_and_print(t1, "1");
    wait_and_print(t2, "2");
    wait_and_print(t3, "3");

    std::cout << "[" << get_current_time() << "] "
              << "****************end****************" << std::endl;
    return 0;
} 