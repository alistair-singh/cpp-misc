
#include <chrono>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <windows.h>

struct TimeEvent {
  long long epoch_ms;
};

auto milli_epoch() {
  using namespace std::chrono;
  TimeEvent event{
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count()};
  return event;
}

VOID CALLBACK timer_tick(PVOID lpParam, BOOLEAN timerOrWaitFired);

class Timer {
  std::function<void(TimeEvent)> action_;
  HANDLE timerHandle_;
  bool disposed_;

  void init(unsigned int startMilli, unsigned int repeatMilli) {
    disposed_ = false;

    if (!::CreateTimerQueueTimer(&timerHandle_, NULL, timer_tick, this,
                                 startMilli, repeatMilli, 0)) {
      throw std::runtime_error("Create Timer Queue Timer failed.");
    }
  }

public:
  Timer()
      : action_(nullptr), timerHandle_(INVALID_HANDLE_VALUE), disposed_(true) {}

  void start(unsigned int startMilli, unsigned int repeatMilli,
             std::function<void(TimeEvent)> action) {
    action_ = action;
    init(startMilli, repeatMilli);
  }

  void start(unsigned int milli, std::function<void(TimeEvent)> action) {
    action_ = action;
    init(milli, milli);
  }

  void once(unsigned int milli, std::function<void(TimeEvent)> action) {
    action_ = action;
    init(milli, 0);
  }

  void tick() const {
    if (action_ != nullptr)
      action_(milli_epoch());
  }

  Timer(const Timer &) = delete;
  Timer(Timer &&other) = delete;

  void stop() {
    if (!disposed_) {
      disposed_ = true;
      (void)DeleteTimerQueueTimer(NULL, timerHandle_, NULL);
    }
  }

  ~Timer() { stop(); }
};

VOID CALLBACK timer_tick(PVOID lpParam, BOOLEAN timerOrWaitFired) {
  if (lpParam != nullptr && timerOrWaitFired) {
    static_cast<Timer *>(lpParam)->tick();
  }
}

int main(int, char **) {
  Timer t1;
  t1.start(200, 200,
           [](auto m) { std::cout << "hello1: " << m.epoch_ms << std::endl; });
  Timer t2;
  t2.once(700,
          [](auto m) { std::cout << "hello2: " << m.epoch_ms << std::endl; });
  std::cout << "timer begin" << std::endl;
  // std::string line;
  // std::getline(std::cin, line);
  Sleep(1000);
  t1.stop();
  Sleep(1000);
  // std::getline(std::cin, line);
  std::cout << "timer end" << std::endl;
  return 0;
}
