#include <gtest/gtest.h>
#include "../include/thread_safe_map.hpp"
#include <thread>
#include <vector>

TEST(MapTest, StartsEmpty) {
    ThreadSafeMap m;
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
}

TEST(MapTest, SetAndGet) {
    ThreadSafeMap m;
    m.set("name", "Joshva");
    auto val = m.get("name");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, "Joshva");
}

TEST(MapTest, GetMissingKey) {
    ThreadSafeMap m;
    auto val = m.get("missing");
    EXPECT_FALSE(val.has_value());
}

TEST(MapTest, UpdateExistingKey) {
    ThreadSafeMap m;
    m.set("name", "Joshva");
    m.set("name", "Updated");
    auto val = m.get("name");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, "Updated");
}

TEST(MapTest, Exists) {
    ThreadSafeMap m;
    m.set("key", "value");
    EXPECT_TRUE(m.exists("key"));
    EXPECT_FALSE(m.exists("missing"));
}

TEST(MapTest, Remove) {
    ThreadSafeMap m;
    m.set("key", "value");
    m.remove("key");
    EXPECT_FALSE(m.exists("key"));
    EXPECT_EQ(m.size(), 0);
}

TEST(MapTest, Clear) {
    ThreadSafeMap m;
    m.set("a", "1");
    m.set("b", "2");
    m.set("c", "3");
    m.clear();
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
}

TEST(MapTest, SizeTracking) {
    ThreadSafeMap m;
    m.set("a", "1");
    m.set("b", "2");
    EXPECT_EQ(m.size(), 2);
    m.remove("a");
    EXPECT_EQ(m.size(), 1);
}

TEST(MapTest, ConcurrentWrites) {
    ThreadSafeMap m;
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++)
        threads.push_back(std::thread([&m, i]{
            m.set("key" + std::to_string(i),
                  "val" + std::to_string(i));
        }));
    for (auto& t : threads) t.join();
    EXPECT_EQ(m.size(), 10);
}

TEST(MapTest, ConcurrentReads) {
    ThreadSafeMap m;
    m.set("name", "Joshva");
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++)
        threads.push_back(std::thread([&m]{
            auto val = m.get("name");
            EXPECT_TRUE(val.has_value());
        }));
    for (auto& t : threads) t.join();
}