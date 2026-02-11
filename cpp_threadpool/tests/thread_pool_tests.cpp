#include <gtest/gtest.h>
#include "thread_pool.hpp"
#include <vector>
#include <atomic>
#include <chrono>

// Test 1: Simple task submission and value retrieval
TEST(ThreadPoolTest, SimpleSubmission) {
    tp::ThreadPool pool(4);
    
    auto future = pool.submit([](int a, int b) {
        return a + b;
    }, 10, 20);

    ASSERT_EQ(future.get(), 30);
}

// Test 2: Concurrent execution check
TEST(ThreadPoolTest, ConcurrentExecution) {
    tp::ThreadPool pool(4);
    std::atomic<int> counter{0};
    int num_tasks = 1000;
    std::vector<std::future<void>> futures;

    for(int i = 0; i < num_tasks; ++i) {
        futures.push_back(pool.submit([&counter](){
            counter.fetch_add(1, std::memory_order_relaxed);
        }));
    }

    // Wait for all
    for(auto& f : futures) {
        f.get();
    }

    ASSERT_EQ(counter.load(), num_tasks);
}

// Test 3: Exception Propagation
TEST(ThreadPoolTest, ExceptionHandling) {
    tp::ThreadPool pool(2);

    auto future = pool.submit([](){
        throw std::runtime_error("Oops");
    });

    ASSERT_THROW(future.get(), std::runtime_error);
}

// Test 4: Shutdown behavior
TEST(ThreadPoolTest, ShutdownBehavior) {
    tp::ThreadPool pool(2);
    
    // Submit a task that sleeps to ensure pool is active
    pool.submit([](){ 
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
    });

    pool.shutdown();

    // Submitting after shutdown should throw
    ASSERT_THROW(pool.submit([](){}), std::runtime_error);
}

// Test 5: Destructor waits for tasks
TEST(ThreadPoolTest, DestructorWaits) {
    std::atomic<bool> flag{false};
    {
        tp::ThreadPool pool(2);
        pool.submit([&flag](){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            flag = true;
        });
    } // Destructor called here. Should wait for sleep to finish.

    ASSERT_TRUE(flag.load());
}
