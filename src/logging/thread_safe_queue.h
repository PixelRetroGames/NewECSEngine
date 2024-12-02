#ifndef THREAD_SAFE_QUEUE
#define THREAD_SAFE_QUEUE

#include <mutex>
#include <condition_variable>
#include <queue>

// ThreadSafeQueue is a Queue with a locking mechanism
template <typename Type>
class ThreadSafeQueue {
  private:
    std::queue<Type> queue;
    std::mutex mutex;
    std::condition_variable condition;
    bool forceUnlocked = false;

  public:
    ThreadSafeQueue() {
    }

    ThreadSafeQueue(const std::queue<Type> &queue) {
        this->queue = queue;
    }

    void push(const Type &val) {
        std::lock_guard lock(mutex);
        queue.push(val);
        condition.notify_one();
    }

    // pop deletes and returns the front element or NULL if the queue is empty
    // pop allocates memory for the element, and returns a pointer to the copy
    // It is up to the caller to manage its memory
    // If the queue is empty, pop will stop the current's thread execution until
    // more elements are pushed or until a forceUnlock is triggered
    Type *pop() {
        std::unique_lock lock(mutex);

        condition.wait(lock, [this]() {
            return !queue.empty() || forceUnlocked;
        });

        if (queue.empty()) {
            return NULL;
        }

        Type *val = new Type;
        *val = queue.front();
        queue.pop();

        return val;
    }

    // forceUnlock signals all threads stopped in pop to stop waiting for new elements,
    // to continue execution and return NULL (the queue is empty)
    void forceUnlock() {
        std::unique_lock lock(mutex);
        forceUnlocked = true;
        lock.unlock();
        condition.notify_all();
    }
};

#endif // THREAD_SAFE_QUEUE

