//
// Created by 吴子庶 on 2021/10/7.
//

#include "stastic_node.h"

namespace Envoy {
namespace Http {
StasticNode::StasticNode() {
  rollingCounterInSecond_ = new ArrayMetric(2, 1000);
  rollingCounterInMinute_ = new ArrayMetric(60, 60 * 1000);
  atomic_init(&curThreadNum_, 0L);
}

StasticNode::~StasticNode() {
  delete rollingCounterInSecond_;
  delete rollingCounterInMinute_;
}

void StasticNode::reset() {
  delete rollingCounterInSecond_;
  rollingCounterInSecond_ = new ArrayMetric(2, 1000);
}

long StasticNode::totalRequest() {
  return rollingCounterInMinute_->pass() + rollingCounterInMinute_->block();
}

long StasticNode::blockRequest() { return rollingCounterInMinute_->block(); }

double StasticNode::blockQps() {
  return rollingCounterInSecond_->block() / rollingCounterInSecond_->getWindowIntervalInSec();
}

double StasticNode::previousBlockQps() { return rollingCounterInMinute_->previousWindowBlock(); }

double StasticNode::previousPassQps() { return rollingCounterInMinute_->previousWindowPass(); }

double StasticNode::totalQps() { return passQps() + blockQps(); }

long StasticNode::totalSuccess() { return rollingCounterInMinute_->success(); }

double StasticNode::exceptionQps() {
  return rollingCounterInSecond_->exception() / rollingCounterInSecond_->getWindowIntervalInSec();
}

long StasticNode::totalException() { return rollingCounterInMinute_->exception(); }

double StasticNode::passQps() {
  return rollingCounterInSecond_->pass() / rollingCounterInSecond_->getWindowIntervalInSec();
}

long StasticNode::totalPass() { return rollingCounterInMinute_->pass(); }

double StasticNode::successQps() {
  return rollingCounterInSecond_->success() / rollingCounterInSecond_->getWindowIntervalInSec();
}

double StasticNode::maxSuccessQps() {
  return rollingCounterInSecond_->maxSuccess() * rollingCounterInSecond_->getSampleCount() /
         rollingCounterInSecond_->getWindowIntervalInSec();
}

double StasticNode::avgRt() {
  long successCount = rollingCounterInSecond_->success();
  if (successCount == 0) {
    return 0;
  }

  return rollingCounterInSecond_->rt() * 1.0 / successCount;
}

double StasticNode::minRt() { return rollingCounterInSecond_->minRt(); }

int StasticNode::curThreadNum() { return curThreadNum_.load(memory_order_relaxed); }

void StasticNode::addPassRequest(int count) {
  rollingCounterInSecond_->addPass(count);
  rollingCounterInMinute_->addPass(count);
}

void StasticNode::addRtAndSuccess(long rt, int successCount) {
  rollingCounterInSecond_->addSuccess(successCount);
  rollingCounterInSecond_->addRT(rt);

  rollingCounterInMinute_->addSuccess(successCount);
  rollingCounterInMinute_->addRT(rt);
}

void StasticNode::increaseBlockQps(int count) {
  rollingCounterInSecond_->addBlock(count);
  rollingCounterInMinute_->addBlock(count);
}

void StasticNode::increaseExceptionQps(int count) {
  rollingCounterInSecond_->addException(count);
  rollingCounterInMinute_->addException(count);
}

void StasticNode::increaseThreadNum() { curThreadNum_++; }

void StasticNode::decreaseThreadNum() { curThreadNum_--; }

StasticNode::StasticNode(const StasticNode& o) {
  rollingCounterInSecond_ = o.rollingCounterInSecond_;
  rollingCounterInMinute_ = o.rollingCounterInMinute_;
  curThreadNum_.store(o.curThreadNum_.load());
}
} // namespace Http

} // namespace Envoy
