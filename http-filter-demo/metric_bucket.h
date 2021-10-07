#pragma once

#include <string>
#include <atomic>
#include "metric_event.h"

using std::atomic_long;

namespace Envoy {
namespace Http {

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
  // MetricBucket(const MetricBucket& o);
  // ~MetricBucket();

  // methods
  void reset();
  long get(Envoy::Http::MetricEvent event);
  void add(MetricEvent event, long n);
  long pass();
  void addPass(int n);
  long block();
  void addBlock(int n);
  long exception();
  void addException(int n);
  long success();
  void addSuccess(int n);
  long rt();
  long minRt();
  void addRt(long rt);

  std::string toString();
};


} // namespace Http
} // namespace Envoy