
#include <iostream>
#include <vector>
#include <random>
#include <ctime>

auto setVector(char val) {
  std::vector<char> v;
  v.resize(1024*1024*1024);
  for(int i = 0; i < v.size(); ++i)
    v[i] = val;
  return v;
}

int main() {
  std::srand(static_cast<int>(time(nullptr)));
  auto randomChar = static_cast<char>(97+(std::rand() % 26));
  auto vector = setVector(randomChar);
  for(const auto &v : vector) {
    std::cout << v << std::endl;
  }
  return 0;
}

