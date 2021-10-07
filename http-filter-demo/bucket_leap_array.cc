#include <string>

#include "bucket_leap_array.h"

namespace Envoy {
namespace Http {

BucketLeapArray::~BucketLeapArray() {}

const MetricBucket BucketLeapArray::newEmptyBucket(long time) {
  printf("time: %ld", time);
  return MetricBucket();
}

WindowWrap<MetricBucket>& BucketLeapArray::resetWindowTo(WindowWrap<MetricBucket>& w,
                                                         long startTime) {
  w.resetTo(startTime);
  w.value().reset();
  return w;
}
} // namespace Http

} // namespace Envoy
