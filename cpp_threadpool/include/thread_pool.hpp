#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include "blocking_queue.hpp"
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <type_traits>
#include <memory>

namespace tp {

class ThreadPool {
public:
    // Starts num_threads worker threads.
    explicit ThreadPool(std::size_t num_threads);

    // Destructor ensures all threads are joined and resources freed.
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Submits a task to the pool.
    // Returns a std::future to retrieve the result or exception.
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using ReturnType = std::invoke_result_t<F, Args...>;

        // 1. Create a packaged_task to wrap the callable and arguments.
        // We use std::bind to bundle arguments with the function.
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        // 2. Get the future associated with the task
        std::future<ReturnType> result = task->get_future();

        // 3. Create a void() lambda that wraps the packaged_task.
        // We capture the shared_ptr to keep the task alive until execution.
        // std::function requires copyable, packaged_task is move-only, hence shared_ptr.
        queue_.push([task]() {
            (*task)();
        });

        return result;
    }

    // Gracefully shuts down the pool. 
    // New submissions fail, but existing tasks are completed.
    void shutdown();

private:
    // The worker loop function
    void worker_loop();

    TaskQueue queue_;
    std::vector<std::thread> workers_;
};

} // namespace tp

#endif // THREAD_POOL_HPP
