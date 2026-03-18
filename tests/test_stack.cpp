#include <gtest/gtest.h>
#include "../include/thread_safe_stack.hpp"
#include <thread>
#include <vector>
#include <atomic>

TEST(StackTest, StartsEmpty) {
    ThreadSafeStack s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
}

TEST(StackTest, SizeAfterPush) {
    ThreadSafeStack s;
    s.push([]{ });
    s.push([]{ });
    s.push([]{ });
    EXPECT_EQ(s.size(), 3);
    EXPECT_FALSE(s.empty());
}

TEST(StackTest, TaskRuns) {
    ThreadSafeStack s;
    int result = 0;
    s.push([&result]{ result = 99; });
    auto task = s.pop();
    ASSERT_TRUE(task.has_value());
    (*task)();
    EXPECT_EQ(result, 99);
}

TEST(StackTest, LIFOOrder) {
    ThreadSafeStack s;
    std::vector<int> order;
    s.push([&order]{ order.push_back(1); });
    s.push([&order]{ order.push_back(2); });
    s.push([&order]{ order.push_back(3); });
    for (int i = 0; i < 3; i++) {
        auto task = s.pop();
        if (task) (*task)();
    }
    EXPECT_EQ(order[0], 3);
    EXPECT_EQ(order[1], 2);
    EXPECT_EQ(order[2], 1);
}

TEST(StackTest, StopReturnsNullopt) {
    ThreadSafeStack s;
    s.stop();
    auto task = s.pop();
    EXPECT_FALSE(task.has_value());
}

TEST(StackTest, PushAfterStop) {
    ThreadSafeStack s;
    s.stop();
    s.push([]{ });
    EXPECT_EQ(s.size(), 0);
}

TEST(StackTest, ConcurrentPushes) {
    ThreadSafeStack s;
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++)
        threads.push_back(std::thread([&s]{
            s.push([]{ });
        }));
    for (auto& t : threads) t.join();
    EXPECT_EQ(s.size(), 10);
}