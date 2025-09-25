#include "../include/queue.hpp"

// Push item into queue
template <typename T>
void TaskQueue<T>::push(const T& item) {
    std::lock_guard<std::mutex> lock(mtx);
    q.push(item);
    cv.notify_one();
}

// Pop item from queue
template <typename T>
T TaskQueue<T>::pop() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] { return !q.empty(); });
    T item = q.front();
    q.pop();
    return item;
}

// Check if queue is empty
template <typename T>
bool TaskQueue<T>::empty() {
    std::lock_guard<std::mutex> lock(mtx);
    return q.empty();
}

// Explicit template instantiation (for common types, e.g. int or std::string)
template class TaskQueue<int>;
template class TaskQueue<std::string>;
template class TaskQueue<std::function<void()>>;   // Needed for ThreadPool
