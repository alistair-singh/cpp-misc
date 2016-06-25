
#include <iostream>

auto returnVal(int i, char c) {
  union {
    int mi;
    char mc;
  } t;

  if (i == 0)
    t.mc = c;
  else
    t.mi = i;

  return t;
}

int main() {
  std::cout << "uniontest.cc\n";

  auto s = returnVal(0, 'a');
  std::cout << s.mc << " " << s.mi << std::endl;
  return 0;
}
