//
// Created by 吴子庶 on 2021/10/6.
//

#pragma once

#include <vector>
#include <array>
#include <thread>

#include <atomic>
#include <mutex>
#include <chrono>

#include "window_wrap.h"

using namespace std;
using namespace chrono;
using std::atomic;
using std::mutex;
using std::string;

namespace Envoy {
    namespace Http {

//        template<typename T>
//        struct WindowWrapAtomic {
//            std::atomic<T> atomic;
//
//            WindowWrapAtomic() : atomic(T()) {}
//
//            WindowWrapAtomic(T &v) : atomic(v) {}
//
//            explicit WindowWrapAtomic(T const &v) : atomic(v) {}
//
//            explicit WindowWrapAtomic(std::atomic<T> const &a) : atomic(a.load()) {}
//
//            WindowWrapAtomic(WindowWrapAtomic const &other) : atomic(other.atomic.load()) {}
//
//            WindowWrapAtomic &operator=(WindowWrapAtomic const &other) {
//                atomic.store(other.atomic.load());
//                return *this;
//            }
//
//            T get() { return atomic.load(); }
//
//            bool compare_exchange_weak(T oldV, T newV) {
//                return atomic.compare_exchange_weak(oldV, newV, memory_order_relaxed);
//            }
//        };

        template<typename T>
        class LeapArray {
        private:
            mutex updateLock_;

            /* data */
            int calculateTimeIdx(long timeMillis);

        protected:
            int windowLengthInMs_;
            int sampleCount_;
            int intervalInMs_;
            double intervalInSecond_;

            shared_ptr<vector<shared_ptr<WindowWrap<T>>>> array_;

            long calculateWindowStart(long timeMillis);

            virtual shared_ptr<WindowWrap<T>> resetWindowTo(shared_ptr<WindowWrap<T>> windowWrap, long startTime) = 0;

        public:
            LeapArray(int sampleCount, int intervalInMs);

            virtual ~LeapArray() {};

            shared_ptr<WindowWrap<T>> currentWindow();

            shared_ptr<WindowWrap<T>> currentWindow(long timeMillis);

            virtual shared_ptr<T> newEmptyBucket(long timeMillis) = 0;

            shared_ptr<WindowWrap<T>> getPreviousWindow(long timeMillis);

            shared_ptr<WindowWrap<T>> getPreviousWindow();

            shared_ptr<T> getWindowValue(long timeMillis);

            bool isWindowDeprecated(shared_ptr<WindowWrap<T>> windowWrap);

            bool isWindowDeprecated(long time, shared_ptr<WindowWrap<T>> windowWrap);

            shared_ptr<vector<shared_ptr<WindowWrap<T>>>> list();

            shared_ptr<vector<shared_ptr<WindowWrap<T>>>> list(long validTime);

            shared_ptr<vector<shared_ptr<WindowWrap<T>>>> listAll();

            shared_ptr<vector<shared_ptr<T>>> values();

            shared_ptr<vector<shared_ptr<T>>> values(long timeMillis);

            shared_ptr<WindowWrap<T>> getValidHead(long timeMillis);

            shared_ptr<WindowWrap<T>> getValidHead();

            int getSampleCount();

            int getIntervalInMs();

            double getIntervalInSecond();
        };


        template<typename T>
        LeapArray<T>::LeapArray(int sampleCount, int intervalInMs) {
//            if (sampleCount <= 0)
//                throw string("bucket count is invalid:") + std::to_string(sampleCount);
//            if (intervalInMs <= 0)
//                throw string("total time interval of the sliding window should be positive");
//            if (intervalInMs % sampleCount == 0)
//                throw string("time span needs to be evenly divided");

            windowLengthInMs_ = intervalInMs / sampleCount;
            intervalInMs_ = intervalInMs;
            intervalInSecond_ = intervalInMs / 1000;
            sampleCount_ = sampleCount;
            array_ = make_shared<vector<shared_ptr<WindowWrap<T>>>>();
            for (int i = 0; i < sampleCount_; i++) {//
                array_->push_back(make_shared<WindowWrap<T>>(windowLengthInMs_, 0L, make_shared<T>()));
            }
        }

//        template<typename T>
//        LeapArray<T>::~LeapArray() {}

        template<typename T>
        int LeapArray<T>::calculateTimeIdx(long timeMillis) {
            long timeId = timeMillis / windowLengthInMs_;
            // Calculate current index so we can map the timestamp to the leap array.
            return timeId % sampleCount_;
        }

        template<typename T>
        long LeapArray<T>::calculateWindowStart(long timeMillis) {
            return timeMillis - timeMillis % windowLengthInMs_;
        }

        template<typename T>
        shared_ptr<WindowWrap<T>> LeapArray<T>::currentWindow() {
            auto ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            return currentWindow(ts);
        }

        template<typename T>
        shared_ptr<WindowWrap<T>> LeapArray<T>::currentWindow(long timeMillis) {

            if (timeMillis < 0) {
                return nullptr;
            }

            int idx = calculateTimeIdx(timeMillis);
            // Calculate current bucket start time.
            long windowStart = calculateWindowStart(timeMillis);

            /*
             * Get bucket item at given time from the array.
             *
             * (1) Bucket is absent, then just create a new bucket and CAS update to circular array.
             * (2) Bucket is up-to-date, then just return the bucket.
             * (3) Bucket is deprecated, then reset current bucket and clean all deprecated buckets.
             */
//            lock_guard<mutex> lock(updateLock_);
            while (true) {
                shared_ptr<WindowWrap<T>> old = array_->at(idx);

//                if (&old == NULL) {
//                    WindowWrap<T> window =
//                            WindowWrap<T>(windowLengthInMs_, windowStart, *newEmptyBucket(timeMillis));
//                    array_->push_back(WindowWrap<T>(window));
//                    return &window;
//                }
                if (old == nullptr || old->value() == nullptr) {
                    /*
                     *     B0       B1      B2    NULL      B4
                     * ||_______|_______|_______|_______|_______||___
                     * 200     400     600     800     1000    1200  timestamp
                     *                             ^
                     *                          time=888
                     *            bucket is empty, so create new and update
                     *
                     * If the old bucket is absent, then we create a new bucket at {@code windowStart},
                     * then try to update circular array via a CAS operation. Only one thread can
                     * succeed to update, while other threads yield its time slice.
                     */


                    if (old == nullptr) {
                        auto window = make_shared<WindowWrap<T>>(
                                WindowWrap<T>(windowLengthInMs_, windowStart, newEmptyBucket(timeMillis)));
                        lock_guard<mutex> lock(updateLock_);
                        array_->at(idx) = window;
                        return array_->at(idx);
                    } else {
                        if (!old->setWindowStart(windowStart)) {
                            continue;
                        }
                        if (!old->setWindowLengthInMs(windowLengthInMs_)) {
                            continue;
                        }
                        if (old->setValue(newEmptyBucket(timeMillis))) {
                            continue;
                        }
                        return array_->at(idx);
                    }
//                    array_->at(idx) = window;
//                    array_->push_back(window);
//                    array_->assign(idx, make_shared<WindowWrap<T>>(
//                            WindowWrap<T>(windowLengthInMs_, windowStart, newEmptyBucket(timeMillis))));
//                    return array_->at(idx);
//                    if (old.compare_exchange_weak(old, window)) {
//                        return window;
//                    } else {
//                        std::this_thread::yield();
//                    }
                } else if (windowStart == old->windowStart()) {
                    /*
                     *     B0       B1      B2     B3      B4
                     * ||_______|_______|_______|_______|_______||___
                     * 200     400     600     800     1000    1200  timestamp
                     *                             ^
                     *                          time=888
                     *            startTime of Bucket 3: 800, so it's up-to-date
                     *
                     * If current {@code windowStart} is equal to the start timestamp of old bucket,
                     * that means the time is within the bucket, so directly return the bucket.
                     */
                    return old;
                } else if (windowStart > old->windowStart()) {
                    /*
                     *   (old)
                     *             B0       B1      B2    NULL      B4
                     * |_______||_______|_______|_______|_______|_______||___
                     * ...    1200     1400    1600    1800    2000    2200  timestamp
                     *                              ^
                     *                           time=1676
                     *          startTime of Bucket 2: 400, deprecated, should be reset
                     *
                     * If the start timestamp of old bucket is behind provided time, that means
                     * the bucket is deprecated. We have to reset the bucket to current {@code windowStart}.
                     * Note that the reset and clean-up operations are hard to be atomic,
                     * so we need a update lock to guarantee the correctness of bucket update.
                     *
                     * The update lock is conditional (tiny scope) and will take effect only when
                     * bucket is deprecated, so in most cases it won't lead to performance loss.
                     */
//                    lock_guard<mutex> lock(updateLock_);
                    auto wrap = resetWindowTo(old, windowStart);
//                    array_->at(idx) = wrap;
                    return wrap;
                } else if (windowStart < old->windowStart()) {
//                    lock_guard<mutex> lock(updateLock_);
                    // Should not go through here, as the provided time is already behind.
                    auto wrap = make_shared<WindowWrap<T>>(windowLengthInMs_, windowStart,
                                                           newEmptyBucket(timeMillis));
                    return wrap;
                }
            }
        }

        template<typename T>
        shared_ptr<WindowWrap<T>> LeapArray<T>::getPreviousWindow(long timeMillis) {
            if (timeMillis < 0) {
                return nullptr;
            }
            int idx = calculateTimeIdx(timeMillis - windowLengthInMs_);
            timeMillis = timeMillis - windowLengthInMs_;
            auto wrap = array_->at(idx);

            if (wrap == nullptr || isWindowDeprecated(wrap)) {
                return nullptr;
            }

            if (wrap->windowStart() + windowLengthInMs_ < (timeMillis)) {
                return nullptr;
            }

            return wrap;
        }

        template<typename T>
        shared_ptr<WindowWrap<T>> LeapArray<T>::getPreviousWindow() {
            auto ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            return getPreviousWindow(ts);
        }

        template<typename T>
        shared_ptr<T> LeapArray<T>::getWindowValue(long timeMillis) {
            if (timeMillis < 0) {
                return nullptr;
            }
            int idx = calculateTimeIdx(timeMillis);

            auto bucket = array_->at(idx);

            if (bucket == nullptr || !bucket->isTimeInWindow(timeMillis)) {
                return nullptr;
            }

            return bucket->value();
        }

        template<typename T>
        bool LeapArray<T>::isWindowDeprecated(shared_ptr<WindowWrap<T>> windowWrap) {
            auto ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            return isWindowDeprecated(ts, windowWrap);
        }

        template<typename T>
        bool LeapArray<T>::isWindowDeprecated(long time, shared_ptr<WindowWrap<T>> windowWrap) {
            return time - windowWrap->windowStart() > intervalInMs_;
        }

        template<typename T>
        shared_ptr<vector<shared_ptr<WindowWrap<T>>>> LeapArray<T>::list(long validTime) {
            int size = array_->size();
            shared_ptr<vector<shared_ptr<WindowWrap<T>>>> result = make_shared<vector<shared_ptr<WindowWrap<T>>>>(size);

            for (int i = 0; i < size; i++) {
                auto windowWrap = array_->at(i);
                if (windowWrap == nullptr || isWindowDeprecated(validTime, windowWrap)) {
                    continue;
                }
                result->at(i) = windowWrap;
            }

            return result;
        }

        template<typename T>
        shared_ptr<vector<shared_ptr<WindowWrap<T>>>> LeapArray<T>::list() {
            auto ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            return list(ts);
        }

        template<typename T>
        shared_ptr<vector<shared_ptr<WindowWrap<T>>>> LeapArray<T>::listAll() {
            int size = array_->size();
            shared_ptr<vector<shared_ptr<WindowWrap<T>>>> result = make_shared<vector<shared_ptr<WindowWrap<T>>>>(size);
            for (int i = 0; i < size; i++) {
                auto windowWrap = array_->at(i);
                if (windowWrap == nullptr) {
                    continue;
                }
                result->at(i) = windowWrap;
            }

            return result;
        }

        template<typename T>
        shared_ptr<vector<shared_ptr<T>>> LeapArray<T>::values(long timeMillis) {
            if (timeMillis < 0) {
                return nullptr;
            }
            int size = array_->size();
            shared_ptr<vector<shared_ptr<T>>> result = make_shared<vector<shared_ptr<T>>>(size);

            for (int i = 0; i < size; i++) {
                shared_ptr<WindowWrap<T>> windowWrap = array_->at(i);
                if (windowWrap == nullptr || isWindowDeprecated(timeMillis, windowWrap)) {
                    continue;
                }
                result->at(i) = windowWrap->value();
            }
            return result;
        }

        template<typename T>
        shared_ptr<vector<shared_ptr<T>>> LeapArray<T>::values() {
            auto ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            return values(ts);
        }

        template<typename T>
        shared_ptr<WindowWrap<T>> LeapArray<T>::getValidHead(long timeMillis) {
            // Calculate index for expected head time.
            int idx = calculateTimeIdx(timeMillis + windowLengthInMs_);

            auto wrap = array_->at(idx);
            if (wrap == nullptr || isWindowDeprecated(wrap)) {
                return nullptr;
            }

            return wrap;
        }

        template<typename T>
        shared_ptr<WindowWrap<T>> LeapArray<T>::getValidHead() {
            auto ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            return getValidHead(ts);
        }

        template<typename T>
        int LeapArray<T>::getSampleCount() { return sampleCount_; }

        template<typename T>
        int LeapArray<T>::getIntervalInMs() { return intervalInMs_; }

        template<typename T>
        double LeapArray<T>::getIntervalInSecond() { return intervalInSecond_; }

    } // namespace Http
} // namespace Envoy
