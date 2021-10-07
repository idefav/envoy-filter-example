#pragma once

#include <string>

#include "leap_array.h"
#include "metric_bucket.h"
#include "source/common/common/logger.h"

using namespace std;

namespace Envoy {
namespace Http {

static MetricBucket emptyMetricBucket;

class BucketLeapArray : public LeapArray<MetricBucket> {
private:
  /* data */
public:
  BucketLeapArray(int sampleCount, int intervalInMs) : LeapArray(sampleCount, intervalInMs){};
  ~BucketLeapArray();

  const MetricBucket newEmptyBucket(long time) override;
  WindowWrap<MetricBucket>& resetWindowTo(WindowWrap<MetricBucket>& w, long startTime) override;
};


} // namespace Http

} // namespace Envoy
