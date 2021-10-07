#pragma once

#include <string>
#include <vector>
#include <array>
#include <thread>

#include <string>
#include <atomic>
#include <mutex>
#include <chrono>

#include "window_wrap.h"

using namespace std;
using namespace chrono;
using std::atomic;
using std::mutex;
using std::string;

namespace Envoy {
namespace Http {

template <typename T> struct WindowWrapAtomic {
  std::atomic<T> atomic;

  WindowWrapAtomic() { std::atomic_init(&atomic, T()); }

  // WindowWrapAtomic(T& v) : atomic(v) {}

  // explicit WindowWrapAtomic(T const& v) : atomic(v) {}
  // explicit WindowWrapAtomic(std::atomic<T> const& a) : atomic(a.load()) {}

  // WindowWrapAtomic(WindowWrapAtomic const& other) : atomic(other.atomic.load()) {}

  WindowWrapAtomic& operator=(WindowWrapAtomic const& other) {
    atomic.store(other.atomic.load());
    return *this;
  }

  T get() { return atomic.load(); }

  bool compare_exchange_weak(T oldV, T newV) {
    return atomic.compare_exchange_weak(oldV, newV, memory_order_relaxed);
  }
};

template <typename T> class LeapArray {
private:
  mutex updateLock_;
  /* data */
  int calculateTimeIdx(long timeMillis);

protected:
  int windowLengthInMs_;
  int sampleCount_;
  int intervalInMs_;
  double intervalInSecond_;

  vector<WindowWrap<T>> array_;

  long calculateWindowStart(long timeMillis);

  virtual WindowWrap<T>& resetWindowTo(WindowWrap<T>& windowWrap, long startTime) = 0;

public:
  LeapArray(int sampleCount, int intervalInMs);
  virtual ~LeapArray();

  WindowWrap<T> currentWindow();

  WindowWrap<T> currentWindow(long timeMillis);

  virtual const T newEmptyBucket(long timeMillis) = 0;

  WindowWrap<T> getPreviousWindow(long timeMillis);

  WindowWrap<T> getPreviousWindow();

  T getWindowValue(long timeMillis);

  bool isWindowDeprecated(WindowWrap<T>& windowWrap);

  bool isWindowDeprecated(long time, WindowWrap<T>& windowWrap);

  vector<WindowWrap<T>> list();
  vector<WindowWrap<T>> list(long validTime);
  vector<WindowWrap<T>> listAll();

  vector<T> values();
  vector<T> values(long timeMillis);

  WindowWrap<T> getValidHead(long timeMillis);
  WindowWrap<T> getValidHead();

  int getSampleCount();
  int getIntervalInMs();
  double getIntervalInSecond();
};

} // namespace Http
} // namespace Envoy