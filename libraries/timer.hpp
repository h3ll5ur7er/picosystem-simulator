#pragma once

#include <functional>
#include <chrono>
#include <future>

class CallBackTimer
{
public:
    CallBackTimer();
    ~CallBackTimer();

    void stop();
    void start(int interval, std::function<void(void)> func);
    bool is_running() const noexcept;

private:
    std::atomic<bool> _execute;
    std::thread _thd;
};