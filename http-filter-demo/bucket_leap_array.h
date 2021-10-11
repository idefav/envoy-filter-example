//
// Created by 吴子庶 on 2021/10/6.
//

#pragma once


#include "leap_array.h"
#include "metric_bucket.h"

using namespace std;

namespace Envoy {
    namespace Http {


        class BucketLeapArray : public LeapArray<MetricBucket> {
        private:
            /* data */
        public:
            BucketLeapArray(int sampleCount, int intervalInMs) : LeapArray(sampleCount, intervalInMs) {};

            ~BucketLeapArray();

            shared_ptr<MetricBucket> newEmptyBucket(long time) override;

            shared_ptr<WindowWrap<MetricBucket>>
            resetWindowTo(shared_ptr<WindowWrap<MetricBucket>> w, long startTime) override;
        };


    } // namespace Http

} // namespace Envoy
