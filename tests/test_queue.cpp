#include <gtest/gtest.h>
#include "../include/thread_safe_queue.hpp"
#include <thread>
#include <vector>
#include <atomic>

TEST(QueueTest, StartsEmpty) {
    ThreadSafeQueue q;
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0);
}

TEST(QueueTest, SizeAfterPush) {
    ThreadSafeQueue q;
    q.push([]{ });
    q.push([]{ });
    q.push([]{ });
    EXPECT_EQ(q.size(), 3);
    EXPECT_FALSE(q.empty());
}

TEST(QueueTest, TaskRuns) {
    ThreadSafeQueue q;
    int result = 0;
    q.push([&result]{ result = 42; });
    auto task = q.pop();
    ASSERT_TRUE(task.has_value());
    (*task)();
    EXPECT_EQ(result, 42);
}

TEST(QueueTest, FIFOOrder) {
    ThreadSafeQueue q;
    std::vector<int> order;
    q.push([&order]{ order.push_back(1); });
    q.push([&order]{ order.push_back(2); });
    q.push([&order]{ order.push_back(3); });
    for (int i = 0; i < 3; i++) {
        auto task = q.pop();
        if (task) (*task)();
    }
    EXPECT_EQ(order[0], 1);
    EXPECT_EQ(order[1], 2);
    EXPECT_EQ(order[2], 3);
}

TEST(QueueTest, StopReturnsNullopt) {
    ThreadSafeQueue q;
    q.stop();
    auto task = q.pop();
    EXPECT_FALSE(task.has_value());
}

TEST(QueueTest, PushAfterStop) {
    ThreadSafeQueue q;
    q.stop();
    q.push([]{ });
    EXPECT_EQ(q.size(), 0);
}

TEST(QueueTest, ConcurrentPushes) {
    ThreadSafeQueue q;
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++)
        threads.push_back(std::thread([&q]{
            q.push([]{ });
        }));
    for (auto& t : threads) t.join();
    EXPECT_EQ(q.size(), 10);
}

TEST(QueueTest, ConcurrentPushPop) {
    ThreadSafeQueue q;
    std::atomic<int> count{0};
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < 5; i++)
        producers.push_back(std::thread([&q]{
            q.push([]{ });
        }));

    for (int i = 0; i < 5; i++)
        consumers.push_back(std::thread([&q, &count]{
            auto task = q.pop();
            if (task) {
                (*task)();
                count++;
            }
        }));

    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    EXPECT_EQ(count, 5);
}