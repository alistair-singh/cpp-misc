
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main() {
  const char someStuff[] = "Some string stuff.";
  const int size = 123;

  char *str = static_cast<char *>(malloc(size));
  printf("before %016zx\n", reinterpret_cast<size_t>(str));
  str = reinterpret_cast<char *>(reinterpret_cast<size_t>(str) |
                                 0x0FFFF00000000000);
  printf("after %016zx\n", reinterpret_cast<size_t>(str));
  memcpy(str, someStuff, sizeof(someStuff));
  puts(str);
  free(str);
  return 0;
}
