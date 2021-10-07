#pragma once

#include <string>
#include <math.h>

#include "metric.h"
#include "metric_bucket.h"
#include "leap_array.h"
#include "bucket_leap_array.h"

using namespace std;

namespace Envoy {
namespace Http {
class ArrayMetric : public Metric {
private:
  /* data */
  LeapArray<MetricBucket>* data_;

public:
  ArrayMetric(int sampleCount, int intervalInMs);
  ~ArrayMetric();

  long success() override;
  long maxSuccess() override;
  long exception() override;
  long block() override;
  long pass() override;
  long rt() override;
  long minRt() override;
  vector<MetricBucket> windows() override;

  void addException(int count) override;
  void addBlock(int count) override;
  void addSuccess(int count) override;
  void addPass(int count) override;
  void addRT(long rt) override;
  long previousWindowBlock() override;
  long previousWindowPass() override;
  void add(MetricEvent event, long count);
  long getCurrentCount(MetricEvent event);
  long getSum(MetricEvent event);
  double getAvg(MetricEvent event);
  long getWindowPass(long timeMillis) override;
  double getWindowIntervalInSec() override;
  int getSampleCount() override;
};

} // namespace Http

} // namespace Envoy
