
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

class Animal {
protected:
  std::string name_;

public:
  Animal(const std::string &name) : name_(name) {}
  std::string name() const { return name_; }
  virtual ~Animal() {}
  virtual std::string toString() const = 0;
};

class Cat : public Animal {
  int age_;

public:
  Cat(const std::string &name, int age) : Animal(name), age_(age) {}
  int age() const { return age_; }

  virtual ~Cat() { std::cout << "dead cat\n"; }
  virtual std::string toString() const override {
    std::ostringstream stream;
    stream << "Cat [" << name_ << " age=" << age_ << "]";
    return stream.str();
  }
};

enum class Color { Black, White, Brown };

std::string toString(Color c) {
  switch (c) {
  case Color::Black:
    return "Black";
  case Color::White:
    return "White";
  case Color::Brown:
    return "Brown";
  default:
    return "unknown";
  }
}

class Dog : public Animal {
  Color color_;

public:
  Dog(const std::string &name, Color color) : Animal(name), color_(color) {}

  virtual ~Dog() { std::cout << "runnaway dog\n"; }
  virtual std::string toString() const override {
    std::ostringstream stream;
    stream << "Dog [" << name_ << " color=" << ::toString(color_) << "]";
    return stream.str();
  }
};

int main() {
  auto dog = std::unique_ptr<Animal>(new Dog("pete", Color::White));
  auto cat = std::unique_ptr<Animal>(new Cat("ace", 12));
  std::cout << "Hello World\n";
  std::cout << "Hello " << dog->toString() << "\n";
  std::cout << "Hello " << cat->toString() << "\n";
  std::cout << "name of dog is " << dog->name() << "\n";
  std::cout << "name of cat is " << cat->name() << "\n";
}
