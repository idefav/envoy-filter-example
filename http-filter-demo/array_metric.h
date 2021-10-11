//
// Created by 吴子庶 on 2021/10/6.
//
#pragma once


#include "metric.h"
#include "bucket_leap_array.h"
#include <algorithm>

using namespace std;

namespace Envoy {
    namespace Http {
        class ArrayMetric : public Metric {
        private:
            /* data */
            shared_ptr<BucketLeapArray> data_;

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

            shared_ptr<vector<shared_ptr<MetricBucket>>> windows() override;

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

