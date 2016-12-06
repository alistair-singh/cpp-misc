
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

template <typename T, typename O> class BufferObserver {
  O mObserver;
  std::vector<T> mBuffer;
  std::mutex mLock;
public:
  BufferObserver(O &observer, int size) : mObserver(observer), mBuffer(size) {}
  void operator () (const T &t) {
    std::lock_guard<std::mutex> t(mLock);
    mBuffer.push_back(t);
    if (mBuffer.size() == mBuffer.capacity()) {
      // do
      reset();
    }
  }
  void reset() {
    mBuffer.clear();
  }
};

int main() {
  std::cout << "H" << std::endl;
  int i = 0;
  BufferObserver<int, int> buffer(i, 10);
  for(i = 0; i < 15; ++i)
    buffer(i);

  buffer.reset();
  return 0;
}
