//---------------------------------------------------------------------------

#pragma once

#ifndef FLOW_TIMER_H
#define FLOW_TIMER_H

//---------------------------------------------------------------------------

#include <flow/context.h>
#include <function/function.h>
#include <boost/bind.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/placeholders.hpp>

//---------------------------------------------------------------------------

namespace asd
{
    namespace flow
    {
        using namespace std::chrono;
        using namespace std::chrono_literals;

        using clock = std::chrono::high_resolution_clock;
        using time_marker = time_point<clock>;
        using ticks_t = long long;

        enum class result
        {
            next,
            stop
        };

        class timer
        {
        public:
            template<class Dur>
            timer(flow::context & flow, Dur duration) : _impl(flow.io(), duration) {}

            template<class Dur, class F>
            timer(flow::context & flow, Dur duration, F callback) : _impl(flow.io(), duration), _callback(callback) {}

            template <class F, useif<is_same<result_of_t<F>, result>::value>>
            void bind(F callback) {
                _callback = callback;
            }

            template <class F, skipif<is_same<result_of_t<F>, result>::value>>
            void bind(F callback) {
                _callback = [=]() {
                    callback();
                    return result::next;
                };
            }

            void start() {
                _impl.async_wait(boost::bind(&timer::update, this, boost::asio::placeholders::error));
            }

        private:
            void update(const boost::system::error_code & /* e */) {
                if (_callback() == result::stop) {
                    return;
                }

                _impl.expires_at(_impl.expiry() + boost::asio::chrono::seconds(1));                
                start();
            }

            boost::asio::steady_timer _impl;
            function<result()> _callback;
        };

        class tick_timer
        {
        public:
            template<class Dur>
            tick_timer(flow::context & flow, Dur duration) : _impl(flow.io(), duration) {}

            template <class F>
            void bind(F callback) {
                _callback = callback;
            }

            void start() {
                _impl.async_wait(boost::bind(&tick_timer::update, this, boost::asio::placeholders::error));
            }

        private:
            void update(const boost::system::error_code & /* e */) {
                _callback(_ticks++);
                _impl.expires_at(_impl.expiry() + boost::asio::chrono::seconds(1));
                start();
            }

            boost::asio::steady_timer _impl;
            function<void(ticks_t)> _callback;
            ticks_t _ticks = 0;
        };
    }
}

//---------------------------------------------------------------------------
#endif
