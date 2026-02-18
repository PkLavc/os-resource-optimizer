#include "timer.h"

namespace osro {

Timer::Timer() : running_(false) {
    reset();
}

void Timer::start() {
    if (!running_) {
        start_time_ = std::chrono::high_resolution_clock::now();
        running_ = true;
    }
}

void Timer::stop() {
    if (running_) {
        end_time_ = std::chrono::high_resolution_clock::now();
        running_ = false;
    }
}

void Timer::reset() {
    start_time_ = std::chrono::high_resolution_clock::now();
    end_time_ = start_time_;
    running_ = false;
}

uint64_t Timer::get_elapsed_milliseconds() const {
    auto end = running_ ? std::chrono::high_resolution_clock::now() : end_time_;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time_);
    return duration.count();
}

uint64_t Timer::get_elapsed_microseconds() const {
    auto end = running_ ? std::chrono::high_resolution_clock::now() : end_time_;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_time_);
    return duration.count();
}

double Timer::get_elapsed_seconds() const {
    auto end = running_ ? std::chrono::high_resolution_clock::now() : end_time_;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_time_);
    return duration.count() / 1000000.0;
}

bool Timer::is_running() const noexcept {
    return running_;
}

} // namespace osro