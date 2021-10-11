//
// Created by 吴子庶 on 2021/10/6.
//

#pragma once

#include <string>
#include <sstream>

using std::ostringstream;
using std::string;
using namespace std;

namespace Envoy {
    namespace Http {
        template<typename T>
        class WindowWrap {
        private:
            /* data */
            long windowLengthInMs_;
            long windowStart_;
            shared_ptr<T> value_;

            mutex lock_;
            atomic_flag flag_ = ATOMIC_FLAG_INIT;

        public:
            WindowWrap() = default;

            WindowWrap(long windowLengthInMs, long windowStart, shared_ptr<T> value)
                    : windowLengthInMs_(windowLengthInMs), windowStart_(windowStart), value_(value) {};

            WindowWrap(long windowLengthInMs, long windowStart)
                    : windowLengthInMs_(windowLengthInMs), windowStart_(windowStart) {};

            WindowWrap(const WindowWrap<T> &win) : windowLengthInMs_(win.windowLengthInMs_),
                                                   windowStart_(win.windowStart_), value_(win.value_) {}

            WindowWrap<T> &operator=(const WindowWrap<T> &o);
//            ~WindowWrap();

            bool setWindowLengthInMs(long windowLengthInMs);

            long windowLength();

            bool setWindowStart(long windowStart);

            long windowStart();

            shared_ptr<T> value();

            bool setValue(shared_ptr<T> v);

            WindowWrap<T> &resetTo(long startTime);

            bool isTimeInWindow(long timeMillis);

            atomic_flag getFlag();

            string toString();
        };

//        template <typename T> WindowWrap<T>::~WindowWrap() {}

        template<typename T>
        long WindowWrap<T>::windowLength() { return windowLengthInMs_; }

        template<typename T>
        bool WindowWrap<T>::setWindowLengthInMs(long windowLengthInMs) {
            if (flag_.test_and_set()) {
                windowLengthInMs_ = windowLengthInMs;
                flag_.clear();
                return true;
            }
            return false;

        }

        template<typename T>
        bool WindowWrap<T>::setWindowStart(long windowStart) {
            if (flag_.test_and_set()) {
                windowStart_ = windowStart;
                flag_.clear();
                return true;
            }
            return false;

        }


        template<typename T>
        long WindowWrap<T>::windowStart() { return windowStart_; }

        template<typename T>
        shared_ptr<T> WindowWrap<T>::value() { return value_; }

        template<typename T>
        bool WindowWrap<T>::setValue(shared_ptr<T> v) {
            if (flag_.test_and_set()) {
                value_ = v;
                flag_.clear();
                return true;
            }
            return false;
        }

        template<typename T>
        WindowWrap<T> &WindowWrap<T>::resetTo(long startTime) {
            windowStart_ = startTime;
            return *this;
        }

        template<typename T>
        bool WindowWrap<T>::isTimeInWindow(long timeMillis) {
            return windowStart_ <= timeMillis && timeMillis < windowStart_ + windowLengthInMs_;
        }

        template<typename T>
        string WindowWrap<T>::toString() {
            ostringstream result;
            result << "WindowWrap{"
                   << "windowLengthInMs=" << windowLengthInMs_ << ", windowStart=" << windowStart_
                   << ", value=" << value_.get() << "}";
            return result.str();
        }

        template<typename T>
        WindowWrap<T> &WindowWrap<T>::operator=(const WindowWrap<T> &o) {
            windowLengthInMs_ = o.windowLengthInMs_;
            windowStart_ = o.windowStart_;
            value_ = o.value_;
            return *this;
        }

    } // namespace Http
} // namespace Envoy

