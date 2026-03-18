#include <gtest/gtest.h>
#include "../include/thread_safe_list.hpp"
#include <thread>
#include <vector>
#include <atomic>

TEST(ListTest, StartsEmpty) {
    ThreadSafeList l;
    EXPECT_TRUE(l.empty());
    EXPECT_EQ(l.size(), 0);
}

TEST(ListTest, SizeAfterPushBack) {
    ThreadSafeList l;
    l.push_back([]{ });
    l.push_back([]{ });
    EXPECT_EQ(l.size(), 2);
    EXPECT_FALSE(l.empty());
}

TEST(ListTest, TaskRuns) {
    ThreadSafeList l;
    int result = 0;
    l.push_back([&result]{ result = 55; });
    auto task = l.pop_front();
    ASSERT_TRUE(task.has_value());
    (*task)();
    EXPECT_EQ(result, 55);
}

TEST(ListTest, PushFrontRunsFirst) {
    ThreadSafeList l;
    std::vector<int> order;
    l.push_back([&order]{ order.push_back(1); });
    l.push_back([&order]{ order.push_back(2); });
    l.push_front([&order]{ order.push_back(99); });
    for (int i = 0; i < 3; i++) {
        auto task = l.pop_front();
        if (task) (*task)();
    }
    EXPECT_EQ(order[0], 99);
    EXPECT_EQ(order[1], 1);
    EXPECT_EQ(order[2], 2);
}

TEST(ListTest, FrontPeek) {
    ThreadSafeList l;
    l.push_back([]{ });
    EXPECT_TRUE(l.front().has_value());
    EXPECT_EQ(l.size(), 1);
}

TEST(ListTest, StopReturnsNullopt) {
    ThreadSafeList l;
    l.stop();
    auto task = l.pop_front();
    EXPECT_FALSE(task.has_value());
}

TEST(ListTest, ConcurrentPushes) {
    ThreadSafeList l;
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++)
        threads.push_back(std::thread([&l]{
            l.push_back([]{ });
        }));
    for (auto& t : threads) t.join();
    EXPECT_EQ(l.size(), 10);
}