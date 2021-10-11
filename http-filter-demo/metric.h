//
// Created by 吴子庶 on 2021/10/6.
//

#pragma once

#include <vector>

#include <memory>
#include "metric_bucket.h"

using namespace std;

namespace Envoy {
    namespace Http {
        class Metric {
        public:
            virtual long success() = 0;

            virtual long maxSuccess() = 0;

            virtual long exception() = 0;

            virtual long block() = 0;

            virtual long pass() = 0;

            virtual long rt() = 0;

            virtual long minRt() = 0;

            virtual shared_ptr<vector<shared_ptr<MetricBucket>>> windows() = 0;

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

            virtual ~Metric() {};
        };


    } // namespace Http

} // namespace Envoy
