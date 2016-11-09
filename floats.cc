
#include <iostream>
#include <cmath>

float pi2(float num) {
  int s = sizeof(double);
  return 67.975437f/num*s;
}

int main(int argc, char**) {
  std::cout << __FILE__ << std::endl;
  std::cout << pi2((float)argc) << std::endl;
  return 0;
}
