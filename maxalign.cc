
#include <iostream>
#include <cstddef>

#pragma warning( push )
#pragma warning( disable : 4324 )
class alignas(8) Person {
public:
  char gender;
  short age; 
  char first_name[9];
  char surname[9];
};

#pragma warning ( pop )

int main() {
  Person p1[] = {
    { 'm', 25, "Alistair", "Singh"},
    { 'f', 24, "Santhana", "Pillay"}
  };

  std::cout << __FILE__ << std::endl;
  std::cout << "sizeof  max_align_t " << sizeof(std::max_align_t) << std::endl;
  std::cout << "alignof max_align_t " << alignof(std::max_align_t) << std::endl;
  std::cout << "alignof int*        " << alignof(int*) << std::endl;
  std::cout << "alignof char        " << alignof(char) << std::endl;
  std::cout << "alignof short       " << alignof(short) << std::endl;
  std::cout << "alignof int         " << alignof(int) << std::endl;
  std::cout << "alignof long        " << alignof(long) << std::endl;
  std::cout << "alignof long long   " << alignof(long long) << std::endl;
  std::cout << "alignof double      " << alignof(double) << std::endl;
  std::cout << "alignof long double " << alignof(long double) << std::endl;
  std::cout << "alignof Person      " << alignof(Person) << std::endl;
  std::cout << "sizeof Person       " << sizeof(Person) << std::endl;

  for(const auto &p : p1) {
    std::cout << p.first_name << " " << p.surname
              << " - " << p.age << " " << p.gender
              << std::endl;
  }
  return 0;
}
