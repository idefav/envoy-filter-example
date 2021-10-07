

#include <string>

#include "array_metric.h"

namespace Envoy {
namespace Http {

ArrayMetric::ArrayMetric(int sampleCount, int intervalInMs) {
  data_ = new BucketLeapArray(sampleCount, intervalInMs);
}

ArrayMetric::~ArrayMetric() { delete data_; }

long ArrayMetric::success() {
  data_->currentWindow();
  long success = 0;

  vector<MetricBucket> list = data_->values();
  for (MetricBucket& window : list) {
    success += window.success();
  }
  return success;
}

long ArrayMetric::maxSuccess() {
  data_->currentWindow();
  long success = 0;

  vector<MetricBucket> list = data_->values();
  for (MetricBucket& window : list) {
    if (window.success() > success) {
      success = window.success();
    }
  }
  return max(success, 1L);
}

long ArrayMetric::exception() {
  data_->currentWindow();
  long exception = 0;
  vector<MetricBucket> list = data_->values();
  for (MetricBucket& window : list) {
    exception += window.exception();
  }
  return exception;
}

long ArrayMetric::block() {
  data_->currentWindow();
  long block = 0;
  vector<MetricBucket> list = data_->values();
  for (MetricBucket& window : list) {
    block += window.block();
  }
  return block;
}

long ArrayMetric::pass() {
  data_->currentWindow();
  long pass = 0;
  vector<MetricBucket> list = data_->values();

  for (MetricBucket& window : list) {
    pass += window.pass();
  }
  return pass;
}

long ArrayMetric::rt() {
  data_->currentWindow();
  long rt = 0;
  vector<MetricBucket> list = data_->values();
  for (MetricBucket& window : list) {
    rt += window.rt();
  }
  return rt;
}

long ArrayMetric::minRt() {
  data_->currentWindow();
  long rt = 5000L;
  vector<MetricBucket> list = data_->values();
  for (MetricBucket& window : list) {
    if (window.minRt() < rt) {
      rt = window.minRt();
    }
  }

  return max(1L, rt);
}

vector<MetricBucket> ArrayMetric::windows() {
  data_->currentWindow();
  return data_->values();
}

void ArrayMetric::addException(int count) {
  WindowWrap<MetricBucket> wrap = data_.currentWindow();
  wrap.value().addException(count);
}

void ArrayMetric::addBlock(int count) {
  WindowWrap<MetricBucket> wrap = data_->currentWindow();
  wrap.value().addBlock(count);
}

void ArrayMetric::addSuccess(int count) {
  WindowWrap<MetricBucket> wrap = data_->currentWindow();
  wrap.value().addSuccess(count);
}

void ArrayMetric::addPass(int count) {
  WindowWrap<MetricBucket> wrap = data_->currentWindow();
  wrap.value().addPass(count);
}

void ArrayMetric::addRT(long rt) {
  WindowWrap<MetricBucket> wrap = data_->currentWindow();
  wrap.value().addRt(rt);
}

long ArrayMetric::previousWindowBlock() {
  data_->currentWindow();
  WindowWrap<MetricBucket> wrap = data_->getPreviousWindow();
  if (wrap == NULL) {
    return 0;
  }
  return wrap.value().block();
}

long ArrayMetric::previousWindowPass() {
  data_->currentWindow();
  auto wrap = data_->getPreviousWindow();
  if (wrap == NULL) {
    return 0;
  }
  return wrap.value().pass();
}

void ArrayMetric::add(MetricEvent event, long count) {
  data_->currentWindow().value().add(event, count);
}

long ArrayMetric::getCurrentCount(MetricEvent event) {
  return data_->currentWindow().value().get(event);
}

long ArrayMetric::getSum(MetricEvent event) {
  data_->currentWindow();
  long sum = 0;

  vector<MetricBucket> buckets = data_->values();
  for (MetricBucket& bucket : buckets) {
    sum += bucket.get(event);
  }
  return sum;
}

double ArrayMetric::getAvg(MetricEvent event) {
  return getSum(event) / data_->getIntervalInSecond();
}

long ArrayMetric::getWindowPass(long timeMillis) {
  MetricBucket bucket = data_->getWindowValue(timeMillis);
  if (bucket == NULL) {
    return 0L;
  }
  return bucket.pass();
}
double ArrayMetric::getWindowIntervalInSec() { return data_->getIntervalInSecond(); }

int ArrayMetric::getSampleCount() { return data_->getSampleCount(); }
} // namespace Http

} // namespace Envoy
