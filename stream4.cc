
#include <iostream>

template <typename TAction> struct Action {
  TAction action;

  template <typename TItem>
  void operator()(TItem item) {
      action(item);
  }
};

struct ActionBuilder {
  template<typename TAction>
  auto operator()(TAction a) {
     return Action<TAction> { std:: move(a) };
  }
};

template <typename TItem>
auto action() {
  return [](TItem){};
}

template <typename TAction, typename TPredicate> struct Where {
  TAction action;
  TPredicate predicate;

  template <typename TItem>
  void operator()(TItem item) {
    if(predicate(item))
      action(item);
  }

  template <typename TAction> 
  auto operator>>(TAction other) {  return other(*this); }
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

//template <typename TBuild1, typename TBuild2>
//auto operator >> (TBuild1 t1, TBuild2 t2) {
//  return 
//}

int main() {
  std::cout << "4" << std::endl;
  
  //auto f = action()([](int i){ std::cout << i << std::endl;});

  auto f =  where([](int i){ return i % 2 == 1; })(
              where([](int i){ return i % 3 == 0; })
                ([](int i){ std::cout << i << std::endl; }));


  f(12);
  f(13);
  f(14);
  f(15);
  f(16);

  return 0;
}

