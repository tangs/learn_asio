#include <iostream>
#include <asio.hpp>
#include <functional>
#include "utils/time_utils.h"

static void print(const std::error_code& /*e*/,
    asio::steady_timer* t, int* count) {
      if (*count < 5)  {
          std::cout << "[" << utils::get_current_time() << "] "
                    << "count:" << *count << std::endl;
          ++(*count);

          t->expires_at(t->expiry() + asio::chrono::seconds(1));
          t->async_wait([t, count](const std::error_code& error) {
              print(error, t, count);
          });
      }
}

int main() {
    asio::io_context io;

    int count = 0;
    asio::steady_timer t(io, asio::chrono::seconds(1));
    t.async_wait([&t, &count](const std::error_code& error) {
        print(error, &t, &count);
    });

    io.run();

    std::cout << "[" << utils::get_current_time() << "] "
              << "Final count is " << count << std::endl;
    return 0;
} 