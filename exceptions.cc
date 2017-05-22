
#include <iostream>
#include <string>
#include <vector>

class M {
  public:
    M(){
      std::cout << "M::M()" << std::endl;
    }
    ~M(){
      std::cout << "M::~M()" << std::endl;
    }
};

int main(int argc, char **argv) {
  std::cout << __FILE__ << std::endl;

  std::vector<std::string> args(argv, argv + argc);
  args.erase(std::begin(args));
  for (const auto &str : args) {
    try {
      M m;
      std::cout << str << " ";
      auto result = std::stoi(str);
      std::cout << "successfully processed: " << result << std::endl;
    } catch (...) {
      std::cout << "error processing: " << str << std::endl;
    }
  }
  std::cout << "fin" << std::endl;

  return 0;
}
