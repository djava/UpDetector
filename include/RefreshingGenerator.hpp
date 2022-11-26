#include <chrono>
#include <coroutine>
#include <exception>
#include <iostream>

namespace chrono {
using namespace std::chrono;

template <class T>
concept Clock_Type = is_clock_v<T>;

template <class T>
concept Duration_Type = std::convertible_to<T, chrono::seconds>;
};  // namespace chrono

// Adapted from: https://www.scs.stanford.edu/~dm/blog/c++-coroutines.html
template <typename T, long Interval_v = 30, chrono::Duration_Type Interval_t = chrono::seconds,
          chrono::Clock_Type Clock = chrono::steady_clock>
struct RefreshingGenerator {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    constexpr static Interval_t Interval {Interval_v};

    struct promise_type {  // required
        T value_;
        std::exception_ptr exception_;

        RefreshingGenerator get_return_object() {
            return RefreshingGenerator(handle_type::from_promise(*this));
        }

        std::suspend_always initial_suspend() { return {}; }

        std::suspend_always final_suspend() noexcept { return {}; }

        void unhandled_exception() { exception_ = std::current_exception(); }  // saving exception

        template <std::convertible_to<T> From>
        std::suspend_always yield_value(From&& from) {
            value_ = std::forward<From>(from);  // caching the result in promise
            return {};
        }

        void return_void() {}
    };

    handle_type h_;

    RefreshingGenerator(handle_type h) : h_(h) {}

    ~RefreshingGenerator() { h_.destroy(); }

    explicit operator bool() {
        fill();  // The only way to reliably find out whether or not we finished coroutine,
                 // whether or not there is going to be a next value generated (co_yield)
                 // in coroutine via C++ getter (operator () below) is to execute/resume
                 // coroutine until the next co_yield point (or let it fall off end).
                 // Then we store/cache result in promise to allow getter (operator() below
                 // to grab it without executing coroutine).
        return !h_.done();
    }

    T operator()() {
        fill();
        return value;
    }

   private:
    T value;
    chrono::time_point<Clock> fill_time {Interval_t::zero()};

    void fill() {
        if (needsFill()) {
            h_();
            this->fill_time = Clock::now();
            if (h_.promise().exception_) std::rethrow_exception(h_.promise().exception_);
            // propagate coroutine exception in called context
            if (!h_.done()) { 
                value = std::move(h_.promise().value_);
            }
        }
    }

    bool needsFill() { return Clock::now() - this->fill_time > Interval; }
};