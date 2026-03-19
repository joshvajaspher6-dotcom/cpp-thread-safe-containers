#include <gtest/gtest.h>
#include "../include/thread_safe_vector.hpp"
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

TEST(VectorTest, StartsEmpty) {
    ThreadSafeVector v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);
}

TEST(VectorTest, SizeAfterPushBack) {
    ThreadSafeVector v;
    v.push_back([]{ });
    v.push_back([]{ });
    EXPECT_EQ(v.size(), 2);
    EXPECT_FALSE(v.empty());
}

TEST(VectorTest, TaskRuns) {
    ThreadSafeVector v;
    int result = 0;
    v.push_back([&result]{ result = 77; });
    auto task = v.pop_back();
    ASSERT_TRUE(task.has_value());
    (*task)();
    EXPECT_EQ(result, 77);
}

TEST(VectorTest, GetByIndex) {
    ThreadSafeVector v;
    v.push_back([]{ });
    v.push_back([]{ });
    v.push_back([]{ });
    EXPECT_TRUE(v.get(0).has_value());
    EXPECT_TRUE(v.get(2).has_value());
    EXPECT_FALSE(v.get(5).has_value());
    EXPECT_FALSE(v.get(-1).has_value());
}

TEST(VectorTest, StopReturnsNullopt) {
    ThreadSafeVector v;
    v.stop();
    auto task = v.pop_back();
    EXPECT_FALSE(task.has_value());
}

TEST(VectorTest, PushAfterStop) {
    ThreadSafeVector v;
    v.stop();
    v.push_back([]{ });
    EXPECT_EQ(v.size(), 0);
}

TEST(VectorTest, ConcurrentPushes) {
    ThreadSafeVector v;
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++)
        threads.push_back(std::thread([&v]{
            v.push_back([]{ });
        }));
    for (auto& t : threads) t.join();
    EXPECT_EQ(v.size(), 10);
}

TEST(VectorTest, WorkerProcessesTasks) {
    ThreadSafeVector v;
    std::atomic<int> count{0};
    std::vector<std::thread> workers;
    for (int i = 0; i < 2; i++)
        workers.push_back(std::thread([&v]{
            while (true) {
                auto task = v.pop_back();
                if (!task) break;
                (*task)();
            }
        }));
    for (int i = 0; i < 10; i++)
        v.push_back([&count]{ count++; });
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    v.stop();
    for (auto& w : workers) w.join();
    EXPECT_EQ(count, 10);
}

TEST(VectorTest, MultipleWorkers) {
    ThreadSafeVector v;
    std::atomic<int> count{0};
    std::vector<std::thread> workers;
    for (int i = 0; i < 4; i++)
        workers.push_back(std::thread([&v]{
            while (true) {
                auto task = v.pop_back();
                if (!task) break;
                (*task)();
            }
        }));
    for (int i = 0; i < 20; i++)
        v.push_back([&count]{ count++; });
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    v.stop();
    for (auto& w : workers) w.join();
    EXPECT_EQ(count, 20);
}

TEST(VectorTest, GetAfterPop) {
    ThreadSafeVector v;
    v.push_back([]{ });
    v.push_back([]{ });
    v.push_back([]{ });
    EXPECT_EQ(v.size(), 3);
    auto task = v.pop_back();
    ASSERT_TRUE(task.has_value());
    EXPECT_EQ(v.size(), 2);
}