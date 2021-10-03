#include <string>
#include <atomic>
#include "metric_event.h"

using std::atomic_long;

namespace Envoy {
namespace Http {
namespace DemoFilter {

class MetricBucket {
private:
  // fields
  atomic_long pass_request_counter;
  atomic_long block_request_counter;
  atomic_long ex_request_counter;
  atomic_long success_request_counter;
  atomic_long rt_counter;
  atomic_long min_rt;

  // methods
  void init();
  void initMinRt();

  long getPassCount();
  long getBlockCount();
  long getExceptionCount();
  long getSuccessCount();
  long getRt();
  long getMinRt();

  void setPassCount(long n);
  void setBlockCount(long n);
  void setExceptionCount(long n);
  void setSuccessCount(long n);
  void setRt(long rt);
  void setMinRt(long rt);

  /* data */
public:
  MetricBucket(/* args */);
  ~MetricBucket();

  // methods
  void reset();
  long get(Envoy::Http::MetricEvent event);
  void add(MetricEvent event, long n);
  long pass();
  void addPass(int n);
  long block();
  void addBlock(int n);
  long exception();
  void addExcetiion(int n);
  long success();
  void addSuccess(int n);
  long rt();
  void addRt(long rt);

  std::string toString();
};

MetricBucket::MetricBucket(/* args */) { init(); }

void MetricBucket::init() {
  std::atomic_init(&pass_request_counter, 0L);
  std::atomic_init(&block_request_counter, 0L);
  std::atomic_init(&ex_request_counter, 0L);
  std::atomic_init(&success_request_counter, 0L);
  std::atomic_init(&rt_counter, 0L);
  std::atomic_init(&min_rt, 5000L);
}

void MetricBucket::reset() { init(); }

MetricBucket::~MetricBucket() {}

// public
long MetricBucket::get(MetricEvent event) {
  switch (event) {
  case MetricEvent::PASS:
    return getPassCount();
  case MetricEvent::BLOCK:
    return getBlockCount();
  case MetricEvent::EXCEPTION:
    return getExceptionCount();
  case MetricEvent::SUCCESS:
    return getSuccessCount();
  case MetricEvent::RT:
    return getRt();
  default:
    break;
  }
  return -1L;
}
void MetricBucket::add(MetricEvent event, long n) {
  switch (event) {
  case MetricEvent::PASS:
    return setPassCount(n);
  case MetricEvent::BLOCK:
    return setBlockCount(n);
  case MetricEvent::EXCEPTION:
    return setExceptionCount(n);
  case MetricEvent::SUCCESS:
    return setSuccessCount(n);
  case MetricEvent::RT:
    return setRt(n);
  default:
    break;
  }
}

long MetricBucket::pass() { return getPassCount(); }
void MetricBucket::addPass(int n) { setPassCount(n); }
long MetricBucket::block() { return getBlockCount(); }
void MetricBucket::addBlock(int n) { setBlockCount(n); }
long MetricBucket::exception() { return getExceptionCount(); }
void MetricBucket::addExcetiion(int n) { setExceptionCount(n); }
long MetricBucket::success() { return getSuccessCount(); }
void MetricBucket::addSuccess(int n) { setSuccessCount(n); }
long MetricBucket::rt() { return getRt(); }
void MetricBucket::addRt(long rt) {
  setRt(rt);
  if (rt < getMinRt()) {
    setMinRt(rt);
  }
}

std::string MetricBucket::toString() {
  std::string pass_str = "p:";
  std::string block_str = "b:";
  std::string comma = ",";
  pass_str += std::to_string(getPassCount());
  block_str += std::to_string(getBlockCount());
  return pass_str + comma + block_str;
}

// private

long MetricBucket::getPassCount() { return pass_request_counter.load(std::memory_order_relaxed); }
long MetricBucket::getBlockCount() { return block_request_counter.load(std::memory_order_relaxed); }
long MetricBucket::getExceptionCount() {
  return ex_request_counter.load(std::memory_order_relaxed);
}
long MetricBucket::getSuccessCount() {
  return success_request_counter.load(std::memory_order_relaxed);
}
long MetricBucket::getRt() { return rt_counter.load(std::memory_order_relaxed); }
long MetricBucket::getMinRt() { return min_rt.load(std::memory_order_relaxed); }

void MetricBucket::setPassCount(long n) { pass_request_counter += n; }
void MetricBucket::setBlockCount(long n) { block_request_counter += n; }
void MetricBucket::setExceptionCount(long n) { ex_request_counter += n; }
void MetricBucket::setSuccessCount(long n) { success_request_counter += n; }
void MetricBucket::setRt(long rt) { rt_counter += rt; }
void MetricBucket::setMinRt(long rt) { min_rt = rt; }

} // namespace DemoFilter
} // namespace Http
} // namespace Envoy