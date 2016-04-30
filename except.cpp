
#include <iostream>

#include <chrono>

using seconds_t = std::chrono::seconds;

auto get_seconds_since_epoch() {
  const auto now = std::chrono::system_clock::now();
  const auto epoch = now.time_since_epoch();
  const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);
  return seconds.count();
}

int doSomethingElse() {
  if (get_seconds_since_epoch()%5 == 3) throw 999;
  return 0;
}

int doSomething() noexcept { return doSomethingElse(); }

int main() {
  std::cout << "except\n";
  doSomething();
  std::cout << "epoch: " << get_seconds_since_epoch() << std::endl;
  return 0;
}
