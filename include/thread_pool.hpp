// thread_pool.hpp
#pragma once
#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include "queue.hpp"

// ThreadPool: pulls tasks from a TaskQueue and executes them
class ThreadPool {
public:
    using Task = std::function<void()>;

    ThreadPool(size_t numThreads, TaskQueue<Task>& q);
    ~ThreadPool();

    // Prevent copying (thread pools should not be copyable)
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    void workerLoop();  // each worker runs this

    TaskQueue<Task>& queue;
    std::vector<std::thread> workers;
    std::atomic<bool> stop;
};
