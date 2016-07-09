
#include <iostream>

struct Pair {
  int first;
  int second;
#ifdef THIRD
  int third;
#endif
};

int main() {
  std::cout << __FILE__ << '\n';
  
  int arr[] = {0,1,2,3,4,5,6,7,8,9};
  int* ptr = &arr[4];
  std::cout << ptr << '\n';
  std::cout << *ptr << '\n';

  Pair* p;
  p = reinterpret_cast<Pair*>(ptr);
  std::cout << "p " << p << '\n';
  std::cout << "f " << p->first << '\n';
  std::cout << "s " << p->second << '\n';
#ifdef THIRD
  std::cout << "t " << p->third << '\n';
#endif
  p = reinterpret_cast<Pair*>(ptr) - 1;
  std::cout << "p " << p << '\n';
  std::cout << "f " << p->first << '\n';
  std::cout << "s " << p->second << '\n';
#ifdef THIRD
  std::cout << "t " << p->third << '\n';
#endif

  char ccc[] = {'a','b','c','d','e','f','g','h','i','j'};
  char* cptr = &ccc[4];
  std::cout << "p " << static_cast<void*>(cptr) << '\n';
  std::cout << "c " << *cptr << '\n';
  cptr = cptr - 1;
  std::cout << "p " << static_cast<void*>(cptr) << '\n';
  std::cout << "c " << *cptr << '\n';

  return 0;
}
