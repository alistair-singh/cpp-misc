#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <ppl.h>

std::vector<int> bigV(int size) {
  std::vector<int> v;
  v.resize(size);

  int i = 0;
  std::generate(begin(v), end(v), [&]() { return ++i; });

  return v;
}

int main() {

  const int SIZE = 80000000;

  std::srand(static_cast<unsigned>(std::time(0)));
  auto random = std::rand();

  try {

    auto madints = bigV(SIZE);
    auto j = bigV(SIZE);

    auto i = 0;
    auto begJ = cbegin(j), endJ = cend(j);
    std::cout << "b\n";
    concurrency::parallel_for_each(cbegin(madints), cend(madints),
                                   [&](const auto &item) {
                                     if (std::binary_search(begJ, endJ, item)) {
                                     }
                                   });

    std::cout << random << '\n';
    std::cout << i << '\n';

    // auto file = std::ofstream("\\users\\alist_000\\data\\madints.bin",
    // std::ios::binary);
    // file.write( (char*) madints.data(), SIZE*4);
  } catch (...) {
    std::cout << "error\n";
  }

  return 0;
}
