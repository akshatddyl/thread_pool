#ifndef BLOCKING_QUEUE_HPP
#define BLOCKING_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <optional>

namespace tp {

// A thread-safe queue specifically for tasks (std::function<void()>)
class TaskQueue {
public:
    TaskQueue() = default;
    ~TaskQueue();

    // Prevent copying/moving to keep synchronization simple
    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;

    // Pushes a task into the queue.
    // Throws if the queue has been stopped.
    void push(std::function<void()> task);

    // Waits for a task and pops it.
    // Returns std::nullopt if the queue is stopped and empty.
    [[nodiscard]] std::optional<std::function<void()>> pop();

    // Signals the queue to reject new items and wake up consumers.
    void shutdown();

    [[nodiscard]] bool empty() const;

private:
    std::queue<std::function<void()>> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool shutdown_requested_{false};
};

} // namespace tp

#endif // BLOCKING_QUEUE_HPP
