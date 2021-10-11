//
// Created by 吴子庶 on 2021/10/6.
//



#include "array_metric.h"

namespace Envoy {
    namespace Http {

        ArrayMetric::ArrayMetric(int sampleCount, int intervalInMs) {
            data_ = make_shared<BucketLeapArray>(sampleCount, intervalInMs);
        }

        ArrayMetric::~ArrayMetric() {}

        long ArrayMetric::success() {
            data_->currentWindow();
            long success = 0;

            auto list = data_->values();
            std::for_each(list->begin(), list->end(), [&](shared_ptr<MetricBucket> &window) {
                if (window == nullptr)return;
                success += window->success();
            });
            return success;
        }

        long ArrayMetric::maxSuccess() {
            data_->currentWindow();
            long success = 0;

            auto list = data_->values();
            std::for_each(list->begin(), list->end(), [&](shared_ptr<MetricBucket> &window) {
                if (window == nullptr)return;
                if (window->success() > success) {
                    success = window->success();
                }
            });
            return max(success, 1L);
        }

        long ArrayMetric::exception() {
            data_->currentWindow();
            long exception = 0;
            auto list = data_->values();
            std::for_each(list->begin(), list->end(), [&](shared_ptr<MetricBucket> &window) {
                if (window == nullptr)return;
                exception += window->exception();
            });
            return exception;
        }

        long ArrayMetric::block() {
            data_->currentWindow();
            long block = 0;
            auto list = data_->values();
            std::for_each(list->begin(), list->end(), [&](shared_ptr<MetricBucket> &window) {
                if (window == nullptr)return;
                block += window->block();
            });
            return block;
        }

        long ArrayMetric::pass() {
            data_->currentWindow();
            long pass = 0;
            auto list = data_->values();


            std::for_each(list->begin(), list->end(), [&](shared_ptr<MetricBucket> &window) {
                if (window == nullptr)return;
                pass += window->pass();
            });
            return pass;
        }

        long ArrayMetric::rt() {
            data_->currentWindow();
            long rt = 0;
            auto list = data_->values();
            std::for_each(list->begin(), list->end(), [&](shared_ptr<MetricBucket> &window) {
                if (window == nullptr)return;
                rt += window->rt();
            });

            return rt;
        }

        long ArrayMetric::minRt() {
            data_->currentWindow();
            long rt = 5000L;
            auto list = data_->values();
            std::for_each(list->begin(), list->end(), [&](shared_ptr<MetricBucket> &window) {
                if (window == nullptr)return;
                if (window->minRt() < rt) {
                    rt = window->minRt();
                }
            });

            return max(1L, rt);
        }

        shared_ptr<vector<shared_ptr<MetricBucket>>> ArrayMetric::windows() {
            data_->currentWindow();
            return data_->values();
        }

        void ArrayMetric::addException(int count) {
            auto wrap = data_->currentWindow();
            wrap->value()->addException(count);
        }

        void ArrayMetric::addBlock(int count) {
            auto wrap = data_->currentWindow();
            wrap->value()->addBlock(count);
        }

        void ArrayMetric::addSuccess(int count) {
            auto wrap = data_->currentWindow();
            wrap->value()->addSuccess(count);
        }

        void ArrayMetric::addPass(int count) {
            auto wrap = data_->currentWindow();
            auto bucket = wrap->value();
            bucket->addPass(count);
            wrap->setValue(bucket);
        }

        void ArrayMetric::addRT(long rt) {
            auto wrap = data_->currentWindow();
            wrap->value()->addRt(rt);
        }

        long ArrayMetric::previousWindowBlock() {
            data_->currentWindow();
            auto wrap = data_->getPreviousWindow();
            if (wrap == nullptr) {
                return 0;
            }
            return wrap->value()->block();
        }

        long ArrayMetric::previousWindowPass() {
            data_->currentWindow();
            auto wrap = data_->getPreviousWindow();
            if (wrap == nullptr) {
                return 0;
            }
            return wrap->value()->pass();
        }

        void ArrayMetric::add(MetricEvent event, long count) {
            data_->currentWindow()->value()->add(event, count);
        }

        long ArrayMetric::getCurrentCount(MetricEvent event) {
            return data_->currentWindow()->value()->get(event);
        }

        long ArrayMetric::getSum(MetricEvent event) {
            data_->currentWindow();
            long sum = 0;

            auto buckets = data_->values();
            std::for_each(buckets->begin(), buckets->end(), [&](shared_ptr<MetricBucket> &bucket) {
                if (bucket == nullptr)return;
                sum += bucket->get(event);
            });
            return sum;
        }

        double ArrayMetric::getAvg(MetricEvent event) {
            return getSum(event) / data_->getIntervalInSecond();
        }

        long ArrayMetric::getWindowPass(long timeMillis) {
            auto bucket = data_->getWindowValue(timeMillis);
            if (bucket == nullptr) {
                return 0L;
            }
            return bucket->pass();
        }

        double ArrayMetric::getWindowIntervalInSec() { return data_->getIntervalInSecond(); }

        int ArrayMetric::getSampleCount() { return data_->getSampleCount(); }
    } // namespace Http

} // namespace Envoy
