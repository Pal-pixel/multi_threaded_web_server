#include "thread_pool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t numThreads) {
    if (numThreads == 0) numThreads = 1;
    workers_.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back([this] { workerLoop(); });
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::submit(TaskQueue::Task t) {
    queue_.push(std::move(t));
}

void ThreadPool::shutdown() {
    bool expected = false;
    if (!stopping_.compare_exchange_strong(expected, true)) return; // already stopped
    queue_.shutdown();

    for (auto& th : workers_) {
        if (th.joinable()) th.join();
    }
}

void ThreadPool::workerLoop() {
    while (!stopping_) {
        auto task = queue_.pop();
        if (!task) {
            // either shutdown or spurious - check stopping
            if (stopping_) break;
            else continue;
        }
        try {
            task();
        } catch (const std::exception& e) {
            std::cerr << "Worker caught exception: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "Worker caught unknown exception\n";
        }
    }
}
