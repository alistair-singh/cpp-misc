
#ifdef WIN32

#include <windows.h>
double get_time() {
  LARGE_INTEGER t, f;
  QueryPerformanceCounter(&t);
  QueryPerformanceFrequency(&f);
  return (double)t.QuadPart / (double)f.QuadPart;
}

#else

#include <sys/resource.h>
#include <sys/time.h>

double get_time() {
  struct timeval t;
  struct timezone tzp;
  gettimeofday(&t, &tzp);
  return t.tv_sec + t.tv_usec * 1e-6;
}

#endif

#include <iostream>
#include <memory>

extern "C" long long movMov(char *buffer, size_t size);
extern "C" long long repMov(char *buffer, size_t size);

const int BufferSize = 1024 * 1024 * 1024;

int main(int argc, char **argv) {

  char *hugeBuffer = 0;
  try {
    hugeBuffer = new char[BufferSize];

    std::cout << "memset:" << std::endl;
    double t1 = get_time();
    memset(hugeBuffer, 0, BufferSize);
    std::cout << 0 << std::endl;
    double t2 = get_time();
    std::cout << "memset: " << (t2 - t1) << ", " << (1.0 * t1) << ", "
              << (1.0 * t2) << std::endl;

    std::cout << "Move:" << std::endl;
    t1 = get_time();
    std::cout << movMov(hugeBuffer, BufferSize) << std::endl;
    t2 = get_time();
    std::cout << "Move: " << (t2 - t1) << ", " << (1.0 * t1) << ", "
              << (1.0 * t2) << std::endl;

    std::cout << "Rep:" << std::endl;
    t1 = get_time();
    std::cout << repMov(hugeBuffer, BufferSize) << std::endl;
    t2 = get_time();
    std::cout << "Rep:  " << (t2 - t1) << ", " << (1.0 * t1) << ", "
              << (1.0 * t2) << std::endl;
  } catch (...) {
  }

  if (!hugeBuffer) {
    delete[] hugeBuffer;
  }
  return 0;
}
