
#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <vector>

std::future<int> main_async(const std::vector<std::string> &args) {
  std::cout << __FILE__ << std::endl;
  std::future<void> fut;
  co_await std::async(std::launch::async, [] {
      std::cout << "Start" << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(10));
      std::cout << "End" << std::endl;
  });
  co_return 0;
}

int main(int argc, char **argv) {
  std::vector<std::string> args(argv, argv + argc);
  return main_async(args).get();
}
