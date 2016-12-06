
#include <functional>
#include <iostream>
#include <vector>

template <typename T> using action = std::function<void(T)>;
template <typename T, typename R> using func = std::function<R(T)>;
template <typename T> using predicate = std::function<bool(T)>;

template <typename T> action<T> never() {
  return [](T) {};
}

template <typename T, typename R> action<R> always(action<T> &a, T t) {
  return [=,&a](R) { a(t); };
}

template <typename T> action<T> buffer(action<std::vector<T>> &a, int size) {
  if (size <= 0)
    return always<std::vector<T>, T>(a, std::vector<T>());
  std::vector<T> buf;
  buf.reserve(size);

  return [=,&a](T t) mutable { // TODO: mutex
    buf.push_back(t);
    if (buf.size() >= size) {
      a(buf);
      buf.clear();
    }
  };
}

template <typename T> action<T> filter(action<T> &&a, predicate<T> &&p) {
  return [&](T t) {
    if (p(t)) {
      a(t);
    }
  };
}

template <typename T, typename R> action<T> map(action<R> &&a, func<T, R> &&m) {
  return [&](T t) { a(m(t)); };
}

template <typename T> action<T> skip(action<T> &&a, int i) {
  if (i <= 0)
    i = 0;
  return [i,&a](T t) mutable { // TODO: mutex
    if (i > 0) {
      i--;
      return;
    }
    a(t);
  };
}

template <typename T> action<T> take(action<T> &&a, int i) {
  if (i < 0)
    i = 0;
  if (i == 0)
    never<T>();
  return [i, &a](T t) mutable { // TODO: mutex
    if (i > 0) {
      i--;
      a(t);
    }
  };
}

template <typename T> action<T> first(action<T> &&a) { return take(a, 1); }

template <typename T> action<T> all(const std::vector<action<T>> &as) {
  return [&as](T t) {
    for (auto a : as)
      a(t);
  };
}

int main() {
  std::function<void(int)> a = [](int i) { std::cout << i << "\n"; };
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
  std::function<void(std::vector<int>)> df = [](std::vector<int> i) {
    if (i.size() >= 2)
      std::cout << i[0] << " - " << i[1] << "\n";
    else
      std::cout << "too small"
                << "\n";
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
  auto f = filter<std::vector<int>>(df, [](auto i) { return i.size() >= 2; });
  auto g = buffer<int>(f, 3);
  g(1);
  g(2);
  g(3);
  g(4);
  g(5);
  //auto h = skip<std::vector<int>>(f, 1);
  //auto g1 = buffer<int>(h, 3);
  //g1(1);
  //g1(2);
  //g1(3);
  //g1(4);
  //g1(5);
  //auto j = map<std::vector<int>, int>(a, [](auto i) { return i[0] + i[1]; });
  //auto j2 = map<std::vector<int>, int>(a, [](auto i) { return i[0] * i[1]; });
  //auto j3 = first<std::vector<int>>(df);
  //auto jj = all<std::vector<int>>({j, j2, j3});
  //auto g2 = buffer<int>(jj, 2);
  //g2(1);
  //g2(2);
  //g2(3);
  //g2(4);
  //g2(5);
  //return 0;
}
