
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

extern "C" long long movMov();
extern "C" long long repMov();

int main(int argc, char **argv) {
  std::cout << "Move:" << std::endl;
  double t1 = get_time();
  std::cout << movMov() << std::endl;
  double t2 = get_time();
  std::cout << "Move: " << (t2 - t1) << ", " << (1.0 * t1) << ", " << (1.0 * t2)
            << std::endl;

  std::cout << "Rep:" << std::endl;
  t1 = get_time();
  std::cout << repMov() << std::endl;
  t2 = get_time();
  std::cout << "Rep:  " << (t2 - t1) << ", " << (1.0 * t1) << ", " << (1.0 * t2)
            << std::endl;
  return 0;
}
