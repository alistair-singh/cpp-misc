
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace stream {
//------------------------------------------------------------- ACTION

template <typename TAction> struct Action {
  TAction action;

  template <typename TItem> void operator()(TItem item) { action(item); }
};

template <typename TAction> Action<TAction> action(TAction action) {
  return Action<TAction>{std::move(action)};
}

//------------------------------------------------------------- ACTION

//------------------------------------------------------------- WHERE

template <typename TAction, typename TPredicate> struct Where {
  TAction action;
  TPredicate predicate;

  template <typename TItem> void operator()(TItem item) {
    if (predicate(item))
      action(item);
  }
};

template <typename TPredicate> struct WhereBuilder {
  TPredicate predicate;
  template <typename TAction> auto operator()(TAction a) {
    return Where<TAction, TPredicate>{std::move(a), std::move(predicate)};
  }
};

template <typename TPredicate> auto where(TPredicate predicate) {
  return WhereBuilder<TPredicate>{std::move(predicate)};
}

//------------------------------------------------------------- WHERE

//------------------------------------------------------------- SELECT

template <typename TAction, typename TMapper> struct Select {
  TAction action;
  TMapper mapper;

  template <typename TItem> void operator()(TItem item) {
    action(mapper(item));
  }
};

template <typename TMapper> struct SelectBuilder {
  TMapper mapper;
  template <typename TAction> auto operator()(TAction a) {
    return Select<TAction, TMapper>{std::move(a), std::move(mapper)};
  }
};

template <typename TMapper> auto select(TMapper mapper) {
  return SelectBuilder<TMapper>{std::move(mapper)};
}

//------------------------------------------------------------- SELECT

//------------------------------------------------------------- COMPOSITION
template <typename TBuild1, typename TBuild2> struct CompositionBuilder {
  TBuild1 first;
  TBuild2 second;
  template <typename TAction> auto operator()(TAction a) {
    return first(second(a));
  }
};

template <typename TBuild1, typename TBuild2>
auto operator>>(TBuild1 t1, TBuild2 t2) {
  return CompositionBuilder<TBuild1, TBuild2>{std::move(t1), std::move(t2)};
}

template <typename TBuild, typename TAction>
auto operator>>(TBuild t, Action<TAction> action) {
  return t(action);
}

//------------------------------------------------------------- COMPOSITION

//------------------------------------------------------------- BUFFER

template <typename TAction, typename TItem> struct Buffer {
  TAction action;
  std::vector<TItem> items;
  // std::mutex lock;

  void operator()(TItem item) {
    // std::lock_guard<std::mutex> guard(lock);
    items.push_back(item);
    if (items.size() >= items.capacity()) {
      action(items);
      items.clear();
      return;
    }
  }

  void Reset() {
    // std::lock_guard<std::mutex> guard(lock);
    items.clear();
    action.reset();
  }
};

template <typename TItem> struct BufferBuilder {
  size_t size;
  template <typename TAction> auto operator()(TAction a) {
    std::vector<TItem> items;
    items.reserve(size);
    return Buffer<TAction, TItem>{std::move(a), std::move(items)};
  }
};

template <typename TItem> auto buffer(size_t size) {
  return BufferBuilder<TItem>{size};
}

//------------------------------------------------------------- BUFFER

//------------------------------------------------------------- WINDOW

template <typename TAction, typename TItem> struct Window {
  TAction action;
  std::vector<TItem> items;
  // std::mutex lock;

  void operator()(TItem item) {
    // std::lock_guard<std::mutex> guard(lock);

    if (items.size() >= items.capacity()) {
      items.erase(items.begin());
    }
    items.push_back(item);
    action(items);
  }

  void Reset() {
    // std::lock_guard<std::mutex> guard(lock);
    items.clear();
    action.reset();
  }
};

template <typename TItem> struct WindowBuilder {
  size_t size;
  template <typename TAction> auto operator()(TAction a) {
    std::vector<TItem> items;
    items.reserve(size);
    return Window<TAction, TItem>{std::move(a), std::move(items)};
  }
};

template <typename TItem> auto window(size_t size) {
  return WindowBuilder<TItem>{size};
}

//------------------------------------------------------------- WINDOW
}

bool sampleFunc(int i) {
  std::cout << "3rd" << std::endl;
  return i % 5 == 0;
}

int main() {
  std::cout << "4\n";

  // auto f = action()([](int i){ std::cout << i << std::endl;});

  auto b = stream::buffer<int>(2) >> 
           stream::window<std::vector<int>>(2) >>
           stream::action([](auto i) {

             std::cout << "[\n";
             for (const auto &si : i) {
               std::cout << "[";
               for (const auto &sii : si) {
                 std::cout << '\t' << sii;
               }
               std::cout << "]\n";
             }
             std::cout << "]\n";

           });
  b(1);
  b(2);
  b(3);
  b(4);
  b(5);
  b(6);
  b(7);
  b(8);
  b(9);
  auto f = stream::where([](auto i) { return i % 2 == 1; }) >>
           stream::where([](auto i) { return i % 3 == 0; }) >>
           stream::where(sampleFunc) >>
           stream::select([](auto i) { return std::to_string(i); }) >>
           stream::action([](const auto &i) {
             std::cout << i.size() << ", " << i << '\n';
           });

  std::function<void(int)> g(f);
  g(12);
  g(13);
  g(14);
  g(15);
  g(16);

  return 0;
}
