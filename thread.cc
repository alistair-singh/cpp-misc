
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

int main() {

  std::vector<std::thread> threads;
  threads.reserve(1000);

  std::cout << "creating\n";
  for (int i = 0; i < 100; ++i) {
    threads.emplace_back(
        []() { std::this_thread::sleep_for(std::chrono::hours(1)); });
  }

  std::cout << "joining\n";
  for (auto &t : threads)
    t.join();

  return 0;
}
