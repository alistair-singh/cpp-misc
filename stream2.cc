
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

template <typename T> using Action = std::function<void(T)>;

template <typename T, typename O> class BufferObserver {
  std::vector<T> mBuffer;
  // std::mutex mLock;
  O &mObserver;

public:
  BufferObserver(O &observer, int size) : mObserver(observer) {
    mBuffer.reserve(size);
  }
  void operator()(const T &t) {
    // std::lock_guard<std::mutex> lock(mLock);
    mBuffer.push_back(t);
    if (mBuffer.size() >= mBuffer.capacity()) {
      mObserver(mBuffer);
      mBuffer.clear();
      return;
    }
  }
  void reset() {
    // std::lock_guard<std::mutex> lock(mLock);
    mBuffer.clear();
    mObserver.reset();
  }
};

template <typename T, typename O> class WindowObserver {
  std::vector<T> mBuffer;
  // std::mutex mLock;
  O &mObserver;

public:
  WindowObserver(O &observer, int size) : mObserver(observer) {
    mBuffer.reserve(size);
  }
  void operator()(const T &t) {
    // std::lock_guard<std::mutex> lock(mLock);
    if (mBuffer.size() >= mBuffer.capacity()) {
      mBuffer.erase(mBuffer.begin());
    }
    mBuffer.push_back(t);
    mObserver(mBuffer);
  }
  void reset() {
    // std::lock_guard<std::mutex> lock(mLock);
    mBuffer.clear();
    mObserver.reset();
  }
};

template <typename T, typename OL, typename OR> class BroadcastObserver {
  OL &mFirst;
  OR &mSecond;

public:
  BroadcastObserver(OL &first, OR &second) : mFirst(first), mSecond(second) {}
  void operator()(const T &t) {
    mFirst(t);
    mSecond(t);
  }
  void reset() {
    mFirst.reset();
    mSecond.reset();
  }
};

template <typename T> class ActionObserver {
  Action<T> mAction;

public:
  ActionObserver(Action<T> action) : mAction(action) {}
  void operator()(const T &t) { mAction(t); }
  void reset() {}
};

template <typename TAction> struct NewAction {
  TAction action;

  template <typename T> void operator()(T t) { action(t); }

  void reset() {}
};

template <typename TAction> auto fromAction(TAction a) {
  return [&] { return NewAction<TAction>{std::move(a)}; };
};

template <typename TPredicate, typename TAction> struct NewPredicate {
  TPredicate predicate;
  TAction action;

  template <typename T> void operator()(T t) {
    if (predicate(T))
      action(t);
  }

  void reset() {}
};

template <typename TAction, typename TPredicate>
auto wherePredicate(TPredicate p) {
  return [&](TAction a) {
    return NewPredicate<TPredicate, TAction>{std::move(p), std::move(a)};
  };
};

//---------------------------------------------------------------------------

template <typename TItem>
struct CallBack {
  void reset() {}
  void next(TItem t) { }

  template <typename TBuilder> auto operator&(TBuilder builder) const {
    return builder.build(*this);
  }
};

template <typename TAction> struct ActionAction {
  TAction action;

  void reset() {}

  template <typename TItem> void next(TItem t) { action(t); }

  template <typename TBuilder> auto operator&(TBuilder builder) const {
    return builder.build(*this);
  }
};

template <typename TAction, typename TPredicate> struct WhereAction {
  TPredicate predicate;
  TAction action;

  void reset() {}

  template <typename TItem> void next(TItem t) {
    if (predicate(t))
      action(t);
  }

  template <typename TBuilder> auto operator&(TBuilder builder) const {
    return builder.build(*this);
  }
};

template <typename TPredicate> struct WhereBuilder {
  TPredicate predicate;

  template <typename TAction>
  WhereAction<TAction, TPredicate> build(TAction action) const {
    return WhereAction<TAction, TPredicate>{predicate, std::move(action)};
  }
};

template <typename TAction> struct ActionBuilder {
  TAction action;

  auto build() const {
    return ActionAction<TAction>{std::move(action)};
  }
};

template <typename TPredicate>
WhereBuilder<TPredicate> where(TPredicate predicate) {
  return WhereBuilder<TPredicate>{std::move(predicate)};
}

template <typename TAction> auto complete(TAction a) {
  return ActionBuilder<TAction>{std::move(a)};
}

template <typename TItem, typename TBuilder> auto callback(TBuilder b) {
  return [&](TItem t) {a;};
}

template <typename TItem, typename TBuilder> auto aaaa(TBuilder b) {
  return [&](TItem t) {a;};
}

int main() {
  //std::cout << "A" << std::endl;
  //ActionObserver<std::vector<char>> dooo{[](const std::vector<char> &v) {
  //  for (const auto &c : v) {
  //    std::cout << "#" << c;
  //  }
  //  std::cout << std::endl;
  //}};

           // where([](const char &c) { return c != 'a'; }) &
           // where([](const char &c) { return c != 'v'; }) &
  auto b = callback<char>() & complete([](char c) { std::cout << c << std::endl; });
  b.next('a');
  b.next('b');
  //char c = 'a';
  //b(c);
  // auto a1 = ActionAction([](const char &c) { std::cout << c << std::endl; });
  // auto a2 = wherePredicate([](const char &c) { return c != 'a'; });

  //BufferObserver<char, ActionObserver<std::vector<char>>> buffer{dooo, 3};
  //WindowObserver<char, ActionObserver<std::vector<char>>> window{dooo, 3};

  //BroadcastObserver<char,
  //                  BufferObserver<char, ActionObserver<std::vector<char>>>,
  //                  WindowObserver<char, ActionObserver<std::vector<char>>>>
  //    broadcast(buffer, window);

  //for (int i = 65; i < 65 + 15; ++i) {
  //  std::cout << "@" << i << std::endl;
  //  broadcast(static_cast<char>(i));
  //}

  //broadcast.reset();
  //return 0;
}
