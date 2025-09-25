#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

// A simple thread-safe queue for tasks (std::function<void()>)
class TaskQueue {
public:
    using Task = std::function<void()>;

    void push(Task t) {
        {
            std::lock_guard<std::mutex> lg(m_);
            q_.push(std::move(t));
        }
        cv_.notify_one();
    }

    // blocks until a task is available, then returns it
    Task pop() {
        std::unique_lock<std::mutex> ul(m_);
        cv_.wait(ul, [this]{ return !q_.empty() || shutdown_; });
        if (q_.empty()) return nullptr; // if shutdown and empty
        Task t = std::move(q_.front());
        q_.pop();
        return t;
    }

    bool empty() {
        std::lock_guard<std::mutex> lg(m_);
        return q_.empty();
    }

    // gracefully wake all waiters and stop
    void shutdown() {
        {
            std::lock_guard<std::mutex> lg(m_);
            shutdown_ = true;
        }
        cv_.notify_all();
    }

private:
    std::queue<Task> q_;
    std::mutex m_;
    std::condition_variable cv_;
    bool shutdown_ = false;
};
