//
// Created by 吴子庶 on 2021/10/6.
//

#include "bucket_leap_array.h"

namespace Envoy {
    namespace Http {

        BucketLeapArray::~BucketLeapArray() {}

        shared_ptr<MetricBucket> BucketLeapArray::newEmptyBucket(long time) {
            return make_shared<MetricBucket>(time);
        }

        shared_ptr<WindowWrap<MetricBucket>> BucketLeapArray::resetWindowTo(shared_ptr<WindowWrap<MetricBucket>> w,
                                                                            long startTime) {
            w->resetTo(startTime);
            const shared_ptr<MetricBucket> &ptr = w->value();
            ptr->reset();
            return w;
        }
    } // namespace Http

} // namespace Envoy