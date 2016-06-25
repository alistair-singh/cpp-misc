
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

auto run1(int num) {
  std::vector<int> v;
  v.reserve(num);
  // std::generate_n(std::back_inserter(v), num, std::rand);
  std::generate_n(std::back_inserter(v), num, [=] { return num * 4; });
  return v;
}

auto run2(int num) {
  std::vector<int> v;
  v.reserve(num);
  for (int i = 0; i < num; i++)
    v.push_back(num * 4);
  return v;
}

int main(int argc, char *argv[]) {
  std::cout << __FILE__ << std::endl;
  if (argc != 2) {
    std::cout << "Need 1 arg." << std::endl;
    return 1;
  }

  try {
    int num = std::stoi(argv[1], nullptr);
    std::cout << "number of items " << num << std::endl;
    auto v1 = run1(num);
    auto v2 = run2(num);
    std::cout << "Length equal " << (v1 == v2) << std::endl;
  } catch (std::exception &e) {
    std::cout << "!!! Exception :" << e.what() << std::endl;
    return 2;
  }
  return 0;
}
