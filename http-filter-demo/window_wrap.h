#pragma once

#include <string>
#include <sstream>

using std::ostringstream;
using std::string;

namespace Envoy {
namespace Http {
template <typename T> class WindowWrap {
private:
  /* data */
  const long windowLengthInMs_;
  long windowStart_;
  T value_;

public:
  WindowWrap() = default;
  WindowWrap(const long windowLengthInMs, long windowStart, T value)
      : windowLengthInMs_(windowLengthInMs), windowStart_(windowStart), value_(value){};
  WindowWrap(const long windowLengthInMs, long windowStart)
      : windowLengthInMs_(windowLengthInMs), windowStart_(windowStart){};
  // ~WindowWrap();

  WindowWrap(const WindowWrap<T>& wrap)
      : windowLengthInMs_(wrap.windowLengthInMs_), windowStart_(wrap.windowStart_),
        value_(wrap.value_) {}

  long windowLength();
  long windowStart();
  T& value();
  void setValue(T v);
  WindowWrap<T>& resetTo(long startTime);
  bool isTimeInWindow(long timeMillis);
  string toString();
};


} // namespace Http
} // namespace Envoy
