
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct SomeType {
  char name[64];
};

struct Pointer1 {
  SomeType *ptr;
  int *ptr2;
};

struct Pointer2 {
  std::unique_ptr<SomeType> ptr;
  std::unique_ptr<int> ptr2;
};

struct Pointer3 {
  std::shared_ptr<SomeType> ptr;
  std::shared_ptr<int> ptr2;
};

int main() {
  std::cout << "p   " << sizeof(int *) << std::endl;
  std::cout << "i   " << sizeof(int) << std::endl;
  std::cout << "s   " << sizeof(std::string) << std::endl;
  std::cout << "vi  " << sizeof(std::vector<int>) << std::endl;
  std::cout << "v   " << sizeof(std::vector<Pointer1>) << std::endl;
  std::cout << "p1  " << sizeof(Pointer1) << std::endl;
  std::cout << "p2  " << sizeof(Pointer2) << std::endl;
  std::cout << "p3  " << sizeof(Pointer3) << std::endl;
  std::cout << "sptr" << sizeof(std::shared_ptr<int>) << std::endl;
  std::cout << "uptr" << sizeof(std::unique_ptr<int>) << std::endl;
  return 0;
}
