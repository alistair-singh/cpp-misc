
#include <iostream>

struct buffer {
  const int size = 1024*8; // 8kb
  char data[size];
};

enum class type {
  I32,
  STR
};

typedef int i32;
struct str {
  unsigned short size;
  char data[1];
};

int main() {
  std::cout << __FILE__ << std::endl;
  return 0;
}
