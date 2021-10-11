//
// Created by 吴子庶 on 2021/10/7.
//

#pragma once


namespace Envoy {
    namespace Http {

        class Node {
        private:
            /* data */
        public:
            virtual long totalRequest() = 0;

            virtual long totalPass() = 0;

            virtual long totalSuccess() = 0;

            virtual long blockRequest() = 0;

            virtual long totalException() = 0;

            virtual double passQps() = 0;

            virtual double blockQps() = 0;

            virtual double totalQps() = 0;

            virtual double successQps() = 0;

            virtual double maxSuccessQps() = 0;

            virtual double exceptionQps() = 0;

            virtual double avgRt() = 0;

            virtual double minRt() = 0;

            virtual int curThreadNum() = 0;

            virtual double previousBlockQps() = 0;

            virtual double previousPassQps() = 0;

            virtual void addPassRequest(int count) = 0;

            virtual void addRtAndSuccess(long rt, int success) = 0;

            virtual void increaseBlockQps(int count) = 0;

            virtual void increaseExceptionQps(int count) = 0;

            virtual void increaseThreadNum() = 0;

            virtual void decreaseThreadNum() = 0;

            virtual void reset() = 0;

            virtual ~Node() {};
        };

    } // namespace Http

} // namespace Envoy
