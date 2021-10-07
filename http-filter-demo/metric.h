#pragma once

#include <string>
#include <vector>

#include "metric_bucket.h"

using namespace std;

namespace Envoy {
namespace Http {
class Metric {
private:
  /* data */
public:
  virtual long success() = 0;
  virtual long maxSuccess() = 0;
  virtual long exception() = 0;
  virtual long block() = 0;
  virtual long pass() = 0;
  virtual long rt() = 0;
  virtual long minRt() = 0;
  virtual vector<MetricBucket> windows() = 0;
  virtual void addException(int n) = 0;
  virtual void addBlock(int n) = 0;
  virtual void addSuccess(int n) = 0;
  virtual void addPass(int n) = 0;
  virtual void addRT(long rt) = 0;
  virtual double getWindowIntervalInSec() = 0;
  virtual int getSampleCount() = 0;
  virtual long getWindowPass(long timeMillis) = 0;
  virtual long previousWindowBlock() = 0;
  virtual long previousWindowPass() = 0;
  virtual ~Metric();
};

Metric::~Metric() {}

} // namespace Http

} // namespace Envoy
