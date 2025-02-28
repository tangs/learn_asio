#include <functional>
#include <iostream>
#include <asio.hpp>


#include "utils/time_utils.h"

class printer {
public:
  explicit printer(asio::io_context& io)
    : timer_(io, asio::chrono::seconds(1)),
      count_(0) {
    std::cout << "[" << utils::get_current_time() << "] "
              << "Start" << std::endl;
    timer_.async_wait([this] (const auto& /*err*/) { print(); });
  }
  ~printer() {
    std::cout << "[" << utils::get_current_time() << "] "
              << "Final count is : " << count_ << std::endl;
  }

public:
  void print() {
    if (count_ >= 5) return;
    std::cout << "[" << utils::get_current_time() << "] "
                  << "count : " << count_ << std::endl;
    ++count_;

    timer_.expires_after(asio::chrono::seconds(1));
    timer_.async_wait([this] (const auto& /*err*/) { print(); });
  }

private:
  asio::steady_timer timer_;
  int count_;
};

int main() {
  asio::io_context io;
  printer p(io);
  io.run();

  return 0;
}
