
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <iostream>

int main() {
  const int SIZE = 1024;

  char *ptr1 = static_cast<char *>(::malloc(sizeof(char) * SIZE));
  auto  ptr2 = new char[SIZE];

  std::fill(ptr1, ptr1 + SIZE, '1');
  std::fill(ptr2, ptr2 + SIZE, '2');

  std::cout << std::hex << static_cast<void *>(ptr1) << '\n'
            << static_cast<void *>(ptr2) << '\n';

  ::free(ptr1);
  delete[] ptr2;
  return 0;
}
