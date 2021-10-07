#pragma once

#include <string>
#include <atomic>

#include "node.h"
#include "metric.h"
#include "array_metric.h"

namespace Envoy {
namespace Http {
class StasticNode : public Node {
private:
  /* data */
  ArrayMetric* rollingCounterInSecond_;
  ArrayMetric* rollingCounterInMinute_;
  atomic_long curThreadNum_;

public:
  StasticNode();

  ~StasticNode();

  // read
  void reset() override;
  long totalRequest() override;
  long blockRequest() override;
  double blockQps() override;
  double previousBlockQps() override;
  double previousPassQps() override;
  double totalQps() override;
  long totalSuccess() override;
  double exceptionQps() override;
  long totalException() override;
  double passQps() override;
  long totalPass() override;
  double successQps() override;
  double maxSuccessQps() override;
  double avgRt() override;
  double minRt() override;
  int curThreadNum() override;

  // write
  void addPassRequest(int count) override;
  void addRtAndSuccess(long rt, int successCount) override;
  void increaseBlockQps(int count) override;
  void increaseExceptionQps(int count) override;
  void increaseThreadNum() override;
  void decreaseThreadNum() override;
};



} // namespace Http

} // namespace Envoy
