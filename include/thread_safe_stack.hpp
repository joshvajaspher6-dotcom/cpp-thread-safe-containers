#pragma once

#include <stack>
#include <mutex>
#include <atomic>
#include <functional>
#include <optional>
#include <condition_variable>

class ThreadSafeStack
{
    private:
        mutable std::mutex mtx_;
        std::condition_variable cv_;
        std::atomic<bool> shutdown_{false};
        std::stack<std::function<void()>> stack_;
    
    public:
        
        void push(std::function<void()> task)
        {
            {
            std::lock_guard<std::mutex>lock(mtx_);
            if (shutdown_)
            {
                return;
            }
            stack_.push(std::move(task));
            }
            cv_.notify_one();
        }

        std::optional<std::function<void()>> pop()
        {
            std::unique_lock<std::mutex>lock(mtx_);
            cv_.wait(lock,[this]
            {
                return !stack_.empty() || shutdown_;
            });
            if (stack_.empty())
                return std::nullopt;
            
            auto task =std::move(stack_.top());
            stack_.pop();
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
            return stack_.empty();
        }

        int size() const
        {
            std::lock_guard<std::mutex>lock(mtx_);
            return stack_.size();
        }
};

