#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

// A threadsafe-queue
// Adapted from https://stackoverflow.com/a/16075550/1604556
template <class T>
class SafeQueue {
private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable cond;
public:
    SafeQueue() : queue(), mutex(), cond() {}
    ~SafeQueue() {}

    // Add an element to the queue
    void enqueue(T val) {
        std::lock_guard<std::mutex> lock(this->mutex);

        this->queue.push(val);
        this->cond.notify_one();
    };

    // Get the "front"-element
    // If the queue is empty, wait until an element is available
    T dequeue(bool wait = true) { // NOTE: `wait` is unimplemented
        std::unique_lock<std::mutex> lock(this->mutex);

        while (this->queue.empty()) {
            //if (!wait)
                //return nullptr;

            // Release lock as long as the wait and reaquire it afterwards.
            this->cond.wait(lock);
        }

        T val = this->queue.front();
        this->queue.pop();

        return val;
    };

    bool empty() {
        // TODO: Not sure if this needs a mutex lock
        return this->queue.empty();
    };
};
