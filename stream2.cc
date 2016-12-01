
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

int main() {
  std::cout << "A" << std::endl;

  ActionObserver<std::vector<char>> dooo{[](const std::vector<char> &v) {
    for (const auto &c : v) {
      std::cout << "#" << c;
    }
    std::cout << std::endl;
  }};
  BufferObserver<char, ActionObserver<std::vector<char>>> buffer{dooo, 3};
  WindowObserver<char, ActionObserver<std::vector<char>>> window{dooo, 3};

  BroadcastObserver<char,
                    BufferObserver<char, ActionObserver<std::vector<char>>>,
                    WindowObserver<char, ActionObserver<std::vector<char>>>>
      broadcast(buffer, window);

  for (int i = 65; i < 65 + 15; ++i) {
    std::cout << "@" << i << std::endl;
    broadcast(static_cast<char>(i));
  }

  broadcast.reset();
  return 0;
}
