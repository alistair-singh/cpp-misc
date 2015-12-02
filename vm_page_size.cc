
#include <Windows.h>

#include <iostream>
#include <algorithm>
#include <memory>

int main(int argc, char** argv) {
  auto fillChar = 'A';
  if(argc > 1)
    fillChar = argv[1][0];

  auto size = GetLargePageMinimum();
  auto regionSize = size * 512 * 2;
  std::cout << size << " B \n";
  std::cout << fillChar << " filler \n";
  auto smallBuffer = std::make_unique<char *>(static_cast<char *>(new char[32]));
  auto bigBuffer = std::make_unique<char *>(static_cast<char *>(new char[regionSize]));

  std::fill(*smallBuffer, *smallBuffer + 32, fillChar);
  std::fill(*bigBuffer, *bigBuffer + regionSize, fillChar);

  std::cout << smallBuffer.get() << "\n";
  std::cout << bigBuffer.get() << "\n";

  return 0;
}
