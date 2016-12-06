
#include <chrono>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <windows.h>

std::chrono::milliseconds milli_epoch() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

VOID CALLBACK timer_tick(PVOID lpParam, BOOLEAN timerOrWaitFired);

class timer {
  std::function<void(std::chrono::milliseconds)> action_;
  HANDLE timerHandle_;
  bool disposed_;

public:
  timer(unsigned int milli,
        std::function<void(std::chrono::milliseconds)> action)
      : timer(milli, milli, action) {}
  timer(unsigned int startMilli, unsigned int repeatMilli,
        std::function<void(std::chrono::milliseconds)> action)
      : action_(action) {
    disposed_ = false;

    if (!::CreateTimerQueueTimer(&timerHandle_, NULL, timer_tick, this,
                                 startMilli, repeatMilli, 0)) {
      throw std::runtime_error("Create Timer Queue Timer failed.");
    }
  }

  static timer once(unsigned int milli,
                    std::function<void(std::chrono::milliseconds)> action) {
    return timer(milli, 0, action);
  }

  void tick() const { action_(milli_epoch()); }

  timer(const timer &) = delete;
  timer(timer &&other) {
    timerHandle_ = other.timerHandle_;
    action_ = other.action_;
    disposed_ = false;
    other.disposed_ = true;
  };

  void stop() {
    if (!disposed_) {
      disposed_ = true;
      DeleteTimerQueueTimer(NULL, timerHandle_, NULL);
    }
  }

  ~timer() { stop(); }
};

VOID CALLBACK timer_tick(PVOID lpParam, BOOLEAN timerOrWaitFired) {
  if (lpParam != nullptr && timerOrWaitFired) {
    static_cast<timer *>(lpParam)->tick();
  }
}

int test(int, char **) {
  timer t{200, [](std::chrono::milliseconds m) {
            std::cout << "hello1: " << m.count() << std::endl;
          }};
  timer t2 = timer::once(700, [](std::chrono::milliseconds m) {
    std::cout << "hello2: " << m.count() << std::endl;
  });
  std::cout << "timer begin" << std::endl;
  // std::string line;
  // std::getline(std::cin, line);
  Sleep(1000);
  t.stop();
  Sleep(1000);
  // std::getline(std::cin, line);
  std::cout << "timer end" << std::endl;
  return 0;
}
