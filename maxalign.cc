
#include <iostream>
#include <cstddef>

int main() {
  std::cout << __FILE__ << std::endl;
  std::cout << "sizeof  max_align_t " << sizeof(std::max_align_t) << std::endl;
  std::cout << "alignof max_align_t " << alignof(std::max_align_t) << std::endl;
  std::cout << "alignof int*        " << alignof(int*) << std::endl;
  std::cout << "alignof char        " << alignof(char) << std::endl;
  std::cout << "alignof short       " << alignof(short) << std::endl;
  std::cout << "alignof int         " << alignof(int) << std::endl;
  std::cout << "alignof long        " << alignof(long) << std::endl;
  std::cout << "alignof long long   " << alignof(long long) << std::endl;
  std::cout << "alignof double      " << alignof(double) << std::endl;
  std::cout << "alignof long double " << alignof(long double) << std::endl;
  return 0;
}
