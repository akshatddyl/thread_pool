#include "blocking_queue.hpp"
#include <stdexcept>

namespace tp {

TaskQueue::~TaskQueue() {
    shutdown();
}

void TaskQueue::push(std::function<void()> task) {
    {
        std::lock_guard lock(mutex_);
        if (shutdown_requested_) {
            throw std::runtime_error("ThreadPool: submit called on stopped queue");
        }
        queue_.push(std::move(task));
    }
    // Notify one waiting worker that work is available
    cv_.notify_one();
}

std::optional<std::function<void()>> TaskQueue::pop() {
    std::unique_lock lock(mutex_);
    
    // Wait until queue is not empty OR shutdown is requested
    // This prevents busy waiting.
    cv_.wait(lock, [this] { 
        return !queue_.empty() || shutdown_requested_; 
    });

    // If we woke up because of shutdown and there is no work left, return empty
    if (queue_.empty() && shutdown_requested_) {
        return std::nullopt;
    }

    // Otherwise, grab the work
    auto task = std::move(queue_.front());
    queue_.pop();
    return task;
}

void TaskQueue::shutdown() {
    {
        std::lock_guard lock(mutex_);
        shutdown_requested_ = true;
    }
    // Wake up ALL workers so they can see the shutdown flag and exit
    cv_.notify_all();
}

bool TaskQueue::empty() const {
    std::lock_guard lock(mutex_);
    return queue_.empty();
}

} // namespace tp
