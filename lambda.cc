
#include <iostream>
#include <memory>

class IWork {
public:
  virtual ~IWork() = default;
  virtual int doWork() = 0;
};

auto doSomeWork = [](int i) -> std::unique_ptr<IWork> {
  struct Annon : public IWork {
    int i_;
    Annon(int i__) : i_(i__) {}
    ~Annon() override { std::cout << "Destruct" << std::endl; }
    int doWork() override {
      return i_++ * i_++;
    }
  };

  return std::unique_ptr<IWork>(new Annon(i));
};

auto doSomeWork2 = [](int i) {
  return [=]() mutable {
    return i++*i++;
  };
};

int main() {
  std::cout << __FILE__ << std::endl;
  auto wo = doSomeWork(23);
  std::cout << wo->doWork() << std::endl;
  std::cout << wo->doWork() << std::endl;
  std::cout << wo->doWork() << std::endl;
  std::cout << wo->doWork() << std::endl;
  std::cout << wo->doWork() << std::endl;
  std::cout << wo->doWork() << std::endl;
  std::cout << wo->doWork() << std::endl;
  std::cout << wo->doWork() << std::endl;
  std::cout << __FILE__ << std::endl;
  auto wo2 = doSomeWork2(23);
  std::cout << wo2() << std::endl;
  std::cout << wo2() << std::endl;
  std::cout << wo2() << std::endl;
  std::cout << wo2() << std::endl;
  std::cout << wo2() << std::endl;
  std::cout << wo2() << std::endl;
  std::cout << wo2() << std::endl;
  std::cout << wo2() << std::endl;
  return 0;
};

