#include <iostream>
#include <asio.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/use_awaitable.hpp>
#include "utils/time_utils.h"

using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
using namespace std::chrono_literals;

int main() {
    try {
        asio::io_context io;
        int count = 0;
        asio::steady_timer timer(io, 1s);

        std::cout << "[" << utils::get_current_time() << "] "
                  << "****************start****************" << std::endl;

        co_spawn(io, 
            [&timer, &count]() -> awaitable<void> {
                try {
                    while (count < 5) {
                        std::cout << "[" << utils::get_current_time() << "] "
                                  << "count: " << count << std::endl;
                        ++count;

                        timer.expires_after(1s);
                        co_await timer.async_wait(use_awaitable);
                    }
                } catch (const std::exception& e) {
                    std::cout << "[" << utils::get_current_time() << "] "
                              << "Timer task exception: " << e.what() << std::endl;
                }
            }(), 
            detached);

        io.run();

        std::cout << "[" << utils::get_current_time() << "] "
                  << "Final count is " << count << std::endl;
        std::cout << "[" << utils::get_current_time() << "] "
                  << "****************end****************" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
} 