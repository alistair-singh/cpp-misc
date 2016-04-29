
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  std::cout << "inspect_vector\n";

  std::vector<char *> args(argv, argv + argc);

  std::cout << sizeof(std::vector<char *>) << std::endl;

  for (const auto &c : args) {
    std::cout << c << std::endl;
  }
}
