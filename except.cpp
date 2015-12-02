
#include <iostream>

int doSomethingElse() noexcept {
  throw 32;
  return 0;
}

int doSomething() noexcept { return doSomethingElse(); }

int main() {
  doSomething();
  std::cout << "except\n";
  return 0;
}
