#include "thread_pool.hpp"

namespace tp {

ThreadPool::ThreadPool(std::size_t num_threads) {
    workers_.reserve(num_threads);
    for (std::size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back(&ThreadPool::worker_loop, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
    // Threads are joined in shutdown/destructor logic to ensure RAII
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::shutdown() {
    queue_.shutdown();
}

void ThreadPool::worker_loop() {
    while (true) {
        // Attempt to get a task
        auto task_opt = queue_.pop();

        // If pop() returns nullopt, it means shutdown was requested 
        // AND the queue is empty.
        if (!task_opt.has_value()) {
            break; 
        }

        // Execute the task. 
        // Exceptions inside the task are caught by std::packaged_task 
        // and stored in the future, so we don't need try/catch here.
        (*task_opt)();
    }
}

} // namespace tp
