// thread_pool.cpp
#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t numThreads, TaskQueue<Task>& q)
    : queue(q), stop(false)
{
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] { workerLoop(); });
    }
}

void ThreadPool::workerLoop() {
    while (true) {
        if (stop) break;

        // Get the next task from the queue (blocks until available)
        Task task = queue.pop();

        // Defensive: skip if task is empty (e.g., shutdown signal)
        if (!task) continue;

        // Process the task
        try {
            task();
        } catch (const std::exception& e) {
            // catch exceptions to prevent worker from dying
            fprintf(stderr, "Worker caught exception: %s\n", e.what());
        } catch (...) {
            fprintf(stderr, "Worker caught unknown exception\n");
        }
    }
}

ThreadPool::~ThreadPool() {
    stop = true;

    // Push dummy tasks to wake all workers so they can exit
    for (size_t i = 0; i < workers.size(); ++i) {
        queue.push([] {});
    }

    for (auto& t : workers) {
        if (t.joinable())
            t.join();
    }
}
