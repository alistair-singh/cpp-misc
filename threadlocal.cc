
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <thread>
#include <future>
#include <functional>

#include <windows.h>

template <typename F>
auto myasync(F && func) {
  std::packaged_task<F> task(func); 

//  ::QueueUserWorkItem(
//      [&](LPVOID) -> DWORD { 
        task();
//        return 0; 
//      }, 
//      nullptr, 
//      0);

  return task.get_future();
}

int main(int argc, char *argv[]) {
  std::vector<std::string>(argv, argv + argc);

  auto res = myasync([]{
    std::cout << "H" << std::endl;
    return true;
  });

  std::cout << "Press enter to exit..." << std::endl;
  std::cin.get();

  return 0;
}
