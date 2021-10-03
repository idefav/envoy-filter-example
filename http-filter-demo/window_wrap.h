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
  WindowWrap(long windowLengthInMs, long windowStart, T value)
      : windowLengthInMs_(windowLengthInMs), windowStart_(windowStart), value_(value){};
  ~WindowWrap();

  long windowLength();
  long windowStart();
  T value();
  void setValue(T v);
  WindowWrap<T> resetTo(long startTime);
  bool isTimeInWindow(long timeMillis);
  string toString();
};

template <typename T> WindowWrap<T>::~WindowWrap() {}

template <typename T> long WindowWrap<T>::windowLength() { return windowLengthInMs_; }
template <typename T> long WindowWrap<T>::windowStart() { return windowStart_; }
template <typename T> T WindowWrap<T>::value() { return value_; }
template <typename T> void WindowWrap<T>::setValue(T v) { value_ = v; }
template <typename T> WindowWrap<T> WindowWrap<T>::resetTo(long startTime) {
  windowStart_ = startTime;
  return this;
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
