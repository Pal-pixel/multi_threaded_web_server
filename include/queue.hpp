#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

// A thread-safe queue (for future threading support, but works fine in single-threaded too)
template <typename T>
class TaskQueue {
public:
    TaskQueue() = default;
    ~TaskQueue() = default;

    // Add an item to the queue
    void push(const T& item);

    // Remove and return the front item
    T pop();

    // Check if queue is empty
    bool empty();

private:
    std::queue<T> q;
    std::mutex mtx;
    std::condition_variable cv;
};

#endif // QUEUE_HPP
