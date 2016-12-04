
#include <iostream>
#include <functional>

template <typename TAction> struct Action {
  TAction action;

  template <typename TItem>
  void operator()(TItem item) {
      action(item);
  }
};

template <typename TAction>
Action<TAction> callable(TAction action) {
  return Action<TAction> { std::move(action) };
}

template <typename TAction, typename TPredicate> struct Where {
  TAction action;
  TPredicate predicate;

  template <typename TItem>
  void operator()(TItem item) {
    if(predicate(item))
      action(item);
  }
};

template <typename TPredicate> struct WhereBuilder {
  TPredicate predicate;
  template<typename TAction>
  auto operator()(TAction a) {
     return Where<TAction,TPredicate> { std::move(a), std::move(predicate) };
  }
};

template <typename TPredicate> auto where(TPredicate predicate) {
  return WhereBuilder<TPredicate> { std::move(predicate) }; 
}

template <typename TBuild1, typename TBuild2>
struct CompositionBuilder {
  TBuild1 first;
  TBuild2 second;
  template<typename TAction>
  auto operator()(TAction a) {
     return first(second(a));
  }
};

template <typename TBuild1, typename TBuild2>
auto operator >> (TBuild1 t1, TBuild2 t2) {
  return CompositionBuilder<TBuild1, TBuild2> { std::move(t1), std::move(t2) };
}

template <typename TBuild, typename TAction>
auto operator >> (TBuild t, Action<TAction> action) {
  return t(action);
}

bool sampleFunc(int i) {
  std::cout << "3rd" << std::endl; 
  return i % 5 == 0;
}

int main() {
  std::cout << "4\n";
  
  //auto f = action()([](int i){ std::cout << i << std::endl;});

  auto f = where([](int i){ return i % 2 == 1; })
         >> where([](int i){ return i % 3 == 0; })
         >> where(sampleFunc)
         >> callable([](int i){ std::cout << i << "\n"; });

  std::function<void(int)> g(f);
  g(12);
  g(13);
  g(14);
  g(15);
  g(16);

  return 0;
}

