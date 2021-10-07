#include <string>

#include "window_wrap.h"

namespace Envoy {
namespace Http {

// template <typename T> WindowWrap<T>::~WindowWrap() {}

template <typename T> long WindowWrap<T>::windowLength() { return windowLengthInMs_; }
template <typename T> long WindowWrap<T>::windowStart() { return windowStart_; }
template <typename T> T& WindowWrap<T>::value() { return value_; }
template <typename T> void WindowWrap<T>::setValue(T v) { value_ = v; }
template <typename T> WindowWrap<T>& WindowWrap<T>::resetTo(long startTime) {
  windowStart_ = startTime;
  return *this;
}
template <typename T> bool WindowWrap<T>::isTimeInWindow(long timeMillis) {
  return windowStart_ <= timeMillis && timeMillis < windowStart_ + windowLengthInMs_;
}
template <typename T> string WindowWrap<T>::toString() {
  ostringstream result;
  result << "WindowWrap{"
         << "windowLengthInMs=" << windowLengthInMs_ << ", windowStart=" << windowStart_
         << ", value=" << value_ << "}";
  return result.str();
}
} // namespace Http

} // namespace Envoy
