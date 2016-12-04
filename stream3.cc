
#include <functional>
#include <iostream>

template <typename TItem, typename TAction> struct Action {
  TAction action;
  void operator()(TItem item) {
      action(item);
  }
};

template <typename TItem, typename TAction1, typename TAction2> struct Action2 {
  TAction1 action1;
  TAction2 action2;
  void operator()(TItem item) {
  }
};

template <typename TItem, typename TAction> auto as_callable(TAction a) {
  return Action<TItem, decltype(a)> { std::move(a) };
}

template <typename TItem> auto action() {
  return [=] (auto a) { 
    return Action<TItem, decltype(a)> { std::move(a) };
  };
}

template <typename TItem, typename TAction, typename TPredicate> struct Where {
  TPredicate predicate;
  TAction action;
  void operator()(TItem item) {
    if (predicate(item)) {
      action(item);
    }
  }
};

template <typename TItem, typename TPredicate>
auto where(TPredicate predicate) {
  return [=](auto action) {
    return Where<TItem, decltype(action), decltype(predicate)>{
        std::move(predicate), std::move(action)};
  };
}


template <typename TA, typename TB> auto operator>>= (TA a, TB b) { return a(b); }

int main() {
  std::cout << "H" << std::endl;

  auto f = action<int>()
           >>= where<int>([](int &i) { return i % 2 == 0; }) 
           >>= where<int>([](int &i) { return i % 3 == 0; }) 
           >>= as_callable<int>([](int &i) { std::cout << i << std::endl; });
  f(12);
  f(23);
  f(9);
  f(2);
  f(12);

  //(action<int>() >> where<int>([](int i) { return (i % 2) == 0; }) >>
  // action<int>([](int i) { std::cout << i << std::endl; }))(12);

  return 0;
}
