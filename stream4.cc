
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

#include "timer_w32.cc"

namespace stream {
//------------------------------------------------------------- ACTION

template <typename TAction> struct Action {
  TAction action;

  template <typename TItem> inline void operator()(TItem item) { action(item); }
  void reset(){}
};

template <typename TAction> Action<TAction> action(TAction &&action) {
  return Action<TAction>{std::forward<TAction>(action)};
}

//------------------------------------------------------------- ACTION

//------------------------------------------------------------- WHERE

template <typename TAction, typename TPredicate> struct Where {
  TAction action;
  TPredicate predicate;

  template <typename TItem> inline void operator()(TItem item) {
    if (predicate(item))
      action(item);
  }
  void reset(){}
};

template <typename TPredicate> struct WhereBuilder {
  TPredicate predicate;
  template <typename TAction> inline auto operator()(TAction &&action) {
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

  template <typename TItem> inline void operator()(TItem item) {
    action(mapper(item));
  }
  void reset(){}
};

template <typename TMapper> struct SelectBuilder {
  TMapper mapper;
  template <typename TAction> inline auto operator()(TAction &&action) {
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
  template <typename TAction> inline auto operator()(TAction &&action) {
    return first(std::move(second(std::forward<TAction>(action))));
  }
};

template <typename TBuild1, typename TBuild2>
inline auto operator>>(TBuild1 &&t1, TBuild2 &&t2) {
  return CompositionBuilder<TBuild1, TBuild2>{std::forward<TBuild1>(t1),
                                              std::forward<TBuild2>(t2)};
}

template <typename TBuild, typename TAction>
inline auto operator>>(TBuild &&t, Action<TAction> &&action) {
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

  inline void operator()(TItem item) {
    std::lock_guard<std::mutex> guard(lock_);
    items_.push_back(item);
    if (items_.size() >= items_.capacity()) {
      action_(items_);
      items_.clear();
      return;
    }
  }

  inline void reset() {
    std::lock_guard<std::mutex> guard(lock_);
    items_.clear();
    action_.reset();
  }
};

template <typename TItem> struct BufferBuilder {
  size_t size;
  template <typename TAction> inline auto operator()(TAction &&action) {
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

  inline void operator()(TItem item) {
    std::lock_guard<std::mutex> guard(lock_);

    if (items_.size() >= items_.capacity()) {
      items_.erase(items_.begin());
    }
    items_.push_back(item);
    action_(items_);
  }

  inline void reset() {
    std::lock_guard<std::mutex> guard(lock_);
    items_.clear();
    action_.reset();
  }
};

template <typename TItem> struct WindowBuilder {
  size_t size;
  template <typename TAction> inline auto operator()(TAction &&action) {
    return Window<TAction, TItem>{std::forward<TAction>(action), size};
  }
};

template <typename TItem> auto window(size_t size) {
  return WindowBuilder<TItem>{size};
}

//------------------------------------------------------------- WINDOW

//------------------------------------------------------------- TIMECLUSTER

template <typename TAction, typename TItem> class TimeCluster {
  TAction action_;
  std::vector<TItem> items_;
  std::recursive_mutex lock_;

  bool first_;
  int milliseconds_;
  Timer timer_;

public:
  TimeCluster(TAction &&action, int milliseconds)
      : action_(std::forward<TAction>(action)), first_(true),
        milliseconds_(milliseconds) {}

  TimeCluster(TimeCluster<TAction, TItem> &&window)
      : action_(std::forward<TAction>(window.action_)),
        items_(std::forward<std::vector<TItem>>(window.items_)),
        first_(window.first_) {}

  inline void operator()(TItem item) {
    std::lock_guard<std::recursive_mutex> guard(lock_);
    if (first_) {
      timer_.once(milliseconds_, [&](auto){flushAndReset();});
      first_ = false;
    }
    items_.push_back(item);
  }

  inline void flushAndReset() {
    std::lock_guard<std::recursive_mutex> guard(lock_);
    if (!items_.empty())
      action_(items_);
    reset();
  }

  inline void reset() {
    std::lock_guard<std::recursive_mutex> guard(lock_);
    timer_.stop();
    items_.clear();
    action_.reset();
    first_ = true;
  }
};

template <typename TItem> struct TimeClusterBuilder {
  int milliseconds;
  template <typename TAction> inline auto operator()(TAction &&action) {
    return TimeCluster<TAction, TItem>{std::forward<TAction>(action), milliseconds};
  }
};

template <typename TItem> auto time_cluster(int milliseconds) {
  return TimeClusterBuilder<TItem>{milliseconds};
}

//------------------------------------------------------------- TIMECLUSTER

//------------------------------------------------------------- CONCAT

template <typename TAction, typename TActionOther> class Concat {
  TAction action_;
  TActionOther other_;

public:
  Concat(TAction &&action, TActionOther &&other)
      : action_(std::forward<TAction>(action)), 
      other_(std::forward<TActionOther>(other)) {}
template <typename TItem>
  inline void operator()(TItem item) {
    action_(item);
    other_(item);
  }

  inline void reset() {
    action_.reset();
    other_.reset();
  }
};

template <typename TActionOther> struct ConcatBuilder {
  TActionOther other;
  template <typename TAction> inline auto operator()(TAction &&action, TActionOther &&other) {
    return Concat<TAction>{std::forward<TAction>(action), std::forward<TActionOther>(other) };
  }
};

template <typename TActionOther> auto concat(TActionOther other) {
  return ConcatBuilder<TActionOther>{other};
}

//------------------------------------------------------------- CONCAT
}

bool sampleFunc(int i) {
  std::cout << "3rd" << std::endl;
  return i % 5 == 0;
}

int main() {
  std::cout << "4\n";

  auto t = stream::time_cluster<int>(500) 
           >> stream::select([](auto i) { 
               std::sort(i.begin(), i.end());
               i.erase(std::unique(i.begin(), i.end()), i.end());
               return i; 
               }) // dedupe
           >> stream::where([](const auto& i) { return !i.empty(); })
           >> stream::concat(stream::action([](const auto& i){
                 std::cout << i.size() << '\n';
                 }))
           >> stream::action([](const auto& i) {
             for (const auto &si : i) {
               std::cout << '\t' << "t(" <<si << ")";
             }
             std::cout << '\n';
           });

  for (int i = 0; i < 20; i++)
  {
    t(i);
    Sleep(70);
  }

  Sleep(1000);
  auto a = stream::action(
      [](int i) { std::cout << "a(" << i << ")" << std::endl; });
  a(12);
  a(34);

  auto b = stream::buffer<int>(2) >> stream::window<std::vector<int>>(2) >>
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
