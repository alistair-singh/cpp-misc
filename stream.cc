
#include <functional>
#include <iostream>
#include <vector>

template <typename T> using action = std::function<void(T)>;
template <typename T> using predicate = std::function<bool(T)>;

template <typename T>
action<T> never() { return [](T){}; }

template <typename T, typename R>
action<R> always(action<T> a, T t) { return [=](R){ a(t); }; } 

template <typename T>
action<T> buffer(action<std::vector<T>> a, int size) {
  if(size <= 0) return always<std::vector<T>,T>(a, std::vector<T>());
  std::vector<T> buf;
  buf.reserve(size);

  return [=](T t) mutable {
    buf.push_back(t);
    if(buf.size() >= size) {
      a(buf);
      buf.clear();
    }
  };
}

template <typename T>
action<T> where(action<T> a, predicate<T> p) {
  return [=](T t) {
    if(p(t)) {
      a(t);
    }
  };
}


int main() {
  auto a = [](int i) { std::cout << i << "\n"; };
  a(1);
  a(2);
  a(3);
  auto b = never<int>();
  b(1);
  b(2);
  b(3);
  auto c = always<int, int>(a, 2);
  c(1);
  c(2);
  c(3);
  auto df = [](std::vector<int> i) { 
    if(i.size() >= 2) std::cout << i[0] << " - " << i[1] << "\n"; 
    else std::cout << "too small" << "\n"; 
  };
  auto d = buffer<int>(df, 2);
  d(1);
  d(2);
  d(3);
  d(4);
  d(5);
  auto e = buffer<int>(df, 0);
  e(1);
  e(2);
  e(3);
  e(4);
  e(5);
  auto f = where<std::vector<int>>(df, [](auto i){ return i.size() >= 2; });
  auto g = buffer<int>(f, 3);
  g(1);
  g(2);
  g(3);
  g(4);
  g(5);
  return 0;
}
