//
// Created by 吴子庶 on 2021/10/6.
//
#include "metric_bucket.h"

//        MetricBucket::MetricBucket(const MetricBucket &o) {
//            pass_request_counter.store(o.pass_request_counter.load());
//            block_request_counter.store(o.block_request_counter.load());
//            ex_request_counter.store(o.ex_request_counter.load());
//            success_request_counter.store(o.success_request_counter.load());
//            rt_counter.store(o.rt_counter.load());
//            min_rt.store(o.min_rt.load());
//        }

namespace Envoy {
    namespace Http {

        void MetricBucket::init() {
            std::atomic_init(&pass_request_counter, 0L);
            std::atomic_init(&block_request_counter, 0L);
            std::atomic_init(&ex_request_counter, 0L);
            std::atomic_init(&success_request_counter, 0L);
            std::atomic_init(&rt_counter, 0L);
            std::atomic_init(&min_rt, 5000L);
        }

        void MetricBucket::reset() { init(); }


// public
        long MetricBucket::get(MetricEvent event) {
            switch (event) {
                case MetricEvent::PASS:
                    return getPassCount();
                case MetricEvent::BLOCK:
                    return getBlockCount();
                case MetricEvent::EXCEPTION:
                    return getExceptionCount();
                case MetricEvent::SUCCESS:
                    return getSuccessCount();
                case MetricEvent::RT:
                    return getRt();
                default:
                    break;
            }
            return -1L;
        }

        void MetricBucket::add(MetricEvent event, long n) {
            switch (event) {
                case MetricEvent::PASS:
                    return setPassCount(n);
                case MetricEvent::BLOCK:
                    return setBlockCount(n);
                case MetricEvent::EXCEPTION:
                    return setExceptionCount(n);
                case MetricEvent::SUCCESS:
                    return setSuccessCount(n);
                case MetricEvent::RT:
                    return setRt(n);
                default:
                    break;
            }
        }

        long MetricBucket::pass() { return getPassCount(); }

        void MetricBucket::addPass(int n) { setPassCount(n); }

        long MetricBucket::block() { return getBlockCount(); }

        void MetricBucket::addBlock(int n) { setBlockCount(n); }

        long MetricBucket::exception() { return getExceptionCount(); }

        void MetricBucket::addException(int n) { setExceptionCount(n); }

        long MetricBucket::success() { return getSuccessCount(); }

        void MetricBucket::addSuccess(int n) { setSuccessCount(n); }

        long MetricBucket::rt() { return getRt(); }

        void MetricBucket::addRt(long rt) {
            setRt(rt);
            if (rt < getMinRt()) {
                setMinRt(rt);
            }
        }

        std::string MetricBucket::toString() {
            std::string pass_str = "p:";
            std::string block_str = "b:";
            std::string comma = ",";
            pass_str += std::to_string(getPassCount());
            block_str += std::to_string(getBlockCount());
            return pass_str + comma + block_str;
        }

//        private

        long MetricBucket::getPassCount() { return pass_request_counter.load(std::memory_order_seq_cst); }

        long MetricBucket::getBlockCount() { return block_request_counter.load(std::memory_order_seq_cst); }

        long MetricBucket::getExceptionCount() {
            return ex_request_counter.load(std::memory_order_seq_cst);
        }

        long MetricBucket::getSuccessCount() {
            return success_request_counter.load(std::memory_order_seq_cst);
        }

        long MetricBucket::getRt() { return rt_counter.load(std::memory_order_seq_cst); }

        long MetricBucket::getMinRt() { return min_rt.load(std::memory_order_seq_cst); }

        void MetricBucket::setPassCount(long n) { pass_request_counter += n; }

        void MetricBucket::setBlockCount(long n) { block_request_counter += n; }

        void MetricBucket::setExceptionCount(long n) { ex_request_counter += n; }

        void MetricBucket::setSuccessCount(long n) { success_request_counter += n; }

        void MetricBucket::setRt(long rt) { rt_counter += rt; }

        void MetricBucket::setMinRt(long rt) { min_rt = rt; }

        long MetricBucket::minRt() { return getMinRt(); }

        MetricBucket::MetricBucket(const MetricBucket &o) {
            pass_request_counter = o.block_request_counter.load();
            block_request_counter = o.block_request_counter.load();
            ex_request_counter = o.ex_request_counter.load();
            success_request_counter = o.success_request_counter.load();
            rt_counter = o.rt_counter.load();
            min_rt = o.min_rt.load();
//            pass_request_counter.store(o.pass_request_counter.load());
//            block_request_counter.store(o.block_request_counter.load());
//            ex_request_counter.store(o.ex_request_counter.load());
//            success_request_counter.store(o.success_request_counter.load());
//            rt_counter.store(o.rt_counter.load());
//            min_rt.store(o.min_rt.load());
            time_ = o.time_;
        }

        MetricBucket &MetricBucket::operator=(const MetricBucket &o) {
            pass_request_counter = o.block_request_counter.load();
            block_request_counter = o.block_request_counter.load();
            ex_request_counter = o.ex_request_counter.load();
            success_request_counter = o.success_request_counter.load();
            rt_counter = o.rt_counter.load();
            min_rt = o.min_rt.load();
//            pass_request_counter.store(o.pass_request_counter.load());
//            block_request_counter.store(o.block_request_counter.load());
//            ex_request_counter.store(o.ex_request_counter.load());
//            success_request_counter.store(o.success_request_counter.load());
//            rt_counter.store(o.rt_counter.load());
//            min_rt.store(o.min_rt.load());
            time_ = o.time_;
            return *this;
        }
    }
}
