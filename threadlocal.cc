
#include <chrono>
#include <iostream>
#include <thread>

class X {
public:
  X() { std::cout << "ctor" << std::endl; }
  ~X() { std::cout << "dtor" << std::endl; }
};

void Do(char *str) {
  thread_local static X x;
  std::cout << &x << " - " << str << std::endl;
}

int main(int argc, char *argv[]) {
  std::cout << __FILE__ << std::endl;

  for (int i = 0; i < argc; i++) {
    std::thread t([=] {
      Do(argv[i]);
      std::this_thread::sleep_for(std::chrono::seconds(i));
      std::cout << "fin " << std::this_thread::get_id() << std::endl;
    });
    t.detach();
  }


  std::cout << "Press enter to exit..." << std::endl;
  std::cin.get();
  return 0;
}
