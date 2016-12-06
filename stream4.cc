
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace stream {
//------------------------------------------------------------- ACTION

template <typename TAction> struct Action {
  TAction action;

  template <typename TItem> void operator()(TItem item) { action(item); }
};

template <typename TAction> Action<TAction> action(TAction &&action) {
  return Action<TAction>{std::forward<TAction>(action)};
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
  template <typename TAction> auto operator()(TAction &&action) {
    return Where<TAction, TPredicate>{std::forward<TAction>(action),
                                      std::move(predicate)};
  }
};

template <typename TPredicate> auto where(TPredicate predicate) {
  return WhereBuilder<TPredicate>{std::forward<TPredicate>(predicate)};
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
  template <typename TAction> auto operator()(TAction &&action) {
    return Select<TAction, TMapper>{std::forward<TAction>(action),
                                    std::move(mapper)};
  }
};

template <typename TMapper> auto select(TMapper &&mapper) {
  return SelectBuilder<TMapper>{std::forward<TMapper>(mapper)};
}

//------------------------------------------------------------- SELECT

//------------------------------------------------------------- COMPOSITION
template <typename TBuild1, typename TBuild2> struct CompositionBuilder {
  TBuild1 first;
  TBuild2 second;
  template <typename TAction> auto operator()(TAction &&action) {
    return first(std::move(second(std::forward<TAction>(action))));
  }
};

template <typename TBuild1, typename TBuild2>
auto operator>>(TBuild1 &&t1, TBuild2 &&t2) {
  return CompositionBuilder<TBuild1, TBuild2>{std::forward<TBuild1>(t1),
                                              std::forward<TBuild2>(t2)};
}

template <typename TBuild, typename TAction>
auto operator>>(TBuild &&t, Action<TAction> &&action) {
  return t(action);
}

//------------------------------------------------------------- COMPOSITION

//------------------------------------------------------------- BUFFER

template <typename TAction, typename TItem> class Buffer {
  TAction action_;
  std::vector<TItem> items_;
  std::mutex lock_;

public:
  Buffer(TAction &&action, size_t size)
      : action_(std::forward<TAction>(action)) {
    items_.reserve(size);
  }

  Buffer(Buffer<TAction, TItem> &&buffer)
      : action_(std::forward<TAction>(buffer.action_)),
        items_(std::forward<std::vector<TItem>>(buffer.items_)) {}

  void operator()(TItem item) {
    std::lock_guard<std::mutex> guard(lock_);
    items_.push_back(item);
    if (items_.size() >= items_.capacity()) {
      action_(items_);
      items_.clear();
      return;
    }
  }

  void Reset() {
    std::lock_guard<std::mutex> guard(lock_);
    items_.clear();
    action_.reset();
  }
};

template <typename TItem> struct BufferBuilder {
  size_t size;
  template <typename TAction> auto operator()(TAction &&action) {
    return Buffer<TAction, TItem>{std::forward<TAction>(action), size};
  }
};

template <typename TItem> auto buffer(size_t size) {
  return BufferBuilder<TItem>{size};
}

//------------------------------------------------------------- BUFFER

//------------------------------------------------------------- WINDOW

template <typename TAction, typename TItem> class Window {
  TAction action_;
  std::vector<TItem> items_;
  std::mutex lock_;

public:
  Window(TAction &&action, size_t size)
      : action_(std::forward<TAction>(action)) {
    items_.reserve(size);
  }

  Window(Window<TAction, TItem> &&window)
      : action_(std::forward<TAction>(window.action_)),
        items_(std::forward<std::vector<TItem>>(window.items_)) {}

  void operator()(TItem item) {
    std::lock_guard<std::mutex> guard(lock_);

    if (items_.size() >= items_.capacity()) {
      items_.erase(items_.begin());
    }
    items_.push_back(item);
    action_(items_);
  }

  void Reset() {
    std::lock_guard<std::mutex> guard(lock_);
    items_.clear();
    action_.reset();
  }
};

template <typename TItem> struct WindowBuilder {
  size_t size;
  template <typename TAction> auto operator()(TAction &&action) {
    return Window<TAction, TItem>{std::forward<TAction>(action), size};
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

  auto a =
      stream::action([](int i) { std::cout << "a(" << i << ")" << std::endl; });
  a(12);
  a(34);

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
           stream::select([](auto i) { return i * 1.1; }) >>
           stream::action([](const auto &i) { std::cout << i << '\n'; });

  std::function<void(int)> g(f);
  g(12);
  g(13);
  g(14);
  g(15);
  g(16);

  return 0;
}
