#pragma once

#include <queue>
#include <mutex>
#include <atomic>
#include <functional>
#include <optional>
#include <condition_variable>

class ThreadSafeQueue
{
    private:
        mutable std::mutex mtx_;
        std::condition_variable cv_;
        std::atomic<bool> shutdown_{false};
        std::queue<std::function<void()>> queue_;
    
    public:
        
        void push(std::function<void()> task)
        {
            {
            std::lock_guard<std::mutex>lock(mtx_);
            if (shutdown_)
            {
                return;
            }
            queue_.push(std::move(task));
            }
            cv_.notify_one();
        }

        std::optional<std::function<void()>> pop()
        {
            std::unique_lock<std::mutex>lock(mtx_);
            cv_.wait(lock,[this]
            {
                return !queue_.empty() || shutdown_;
            });
            if (queue_.empty())
                return std::nullopt;
            
            auto task =std::move(queue_.front());
            queue_.pop();
            return task;
        }

        void stop()
        {
            shutdown_=true;
            cv_.notify_all();
        }

        bool empty() const
        {
            std::lock_guard<std::mutex>lock(mtx_);
            return queue_.empty();
        }

        int size() const
        {
            std::lock_guard<std::mutex>lock(mtx_);
            return queue_.size();
        }
};

