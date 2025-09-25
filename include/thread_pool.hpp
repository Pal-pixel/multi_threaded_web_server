#pragma once
#include "task_queue.hpp"
#include <thread>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    // push a task into the queue
    void submit(TaskQueue::Task t);

    // stop the pool and join threads
    void shutdown();

private:
    void workerLoop();

    TaskQueue queue_;
    std::vector<std::thread> workers_;
    std::atomic<bool> stopping_{false};
};
