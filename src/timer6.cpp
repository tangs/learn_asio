#include <functional>
#include <iostream>
#include <thread>
#include <asio.hpp>

#include "utils/time_utils.h"

class printer {
public:
  explicit printer(asio::io_context& io)
    : strand_(asio::make_strand(io))
    , timer1_(io, asio::chrono::seconds(1))
  , timer2_(io, asio::chrono::seconds(1)) {
    std::cout << "[" << utils::get_current_time() << "] "
              << "[" << std::this_thread::get_id() << "]"
              << "constrctor" << std::endl;

    timer1_.async_wait(asio::bind_executor(strand_, [this] (const auto&) {
      print1();
    }));
    timer2_.async_wait(asio::bind_executor(strand_, [this] (const auto&) {
      print2();
    }));
  }
  ~printer() {
    std::cout << "[" << utils::get_current_time() << "] "
              << "[" << std::this_thread::get_id() << "]"
              << "Final count is : " << count_ << std::endl;
  }
public:
  void print1() {
    if (count_ >= 10) return;

    std::cout << "[" << utils::get_current_time() << "] "
              << "[" << std::this_thread::get_id() << "]"
              << "Timer 1: " << count_ << std::endl;
    ++count_;

    timer1_.expires_after(asio::chrono::seconds(1));

    timer1_.async_wait(asio::bind_executor(strand_,
            [this] (const auto& err) {
              this->print1();
            }));
  }

  void print2() {
    if (count_ >= 10) return;

    std::cout << "[" << utils::get_current_time() << "] "
              << "[" << std::this_thread::get_id() << "]"
              << "Timer 2: " << count_ << std::endl;
    ++count_;

    timer2_.expires_after(asio::chrono::milliseconds(1500));

    timer2_.async_wait(asio::bind_executor(strand_,
            [this] (const auto& err) {
              this->print2();
            }));
  }
private:
  asio::strand<asio::io_context::executor_type> strand_;
  asio::steady_timer timer1_;
  asio::steady_timer timer2_;
  int count_ = 0;
};

int main() {
  std::cout << "[" << utils::get_current_time() << "] "
            << "[" << std::this_thread::get_id() << "]"
            << "Start" << std::endl;

  asio::io_context io;
  printer p(io);
  std::thread t([&]{ io.run(); });

  io.run();
  t.join();

  return 0;
}