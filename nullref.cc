
#include <iostream>

class Test {
  public:
    int doSomeWork(int i) {
      std::cout << "someWork\n";
      return i*i;
    }
};

int main() {
  std::cout << __FILE__ << '\n';
  Test *t = nullptr;
  auto result = t->doSomeWork(34);
  std::cout << result << '\n';
  return 0;
}
