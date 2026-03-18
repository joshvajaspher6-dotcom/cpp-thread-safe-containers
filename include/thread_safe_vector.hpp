#pragma once

#include <vector>
#include <mutex>
#include <atomic>
#include <functional>
#include <optional>
#include <condition_variable>

class ThreadSafeVector
{
    private:
        mutable std::mutex mtx_;
        std::condition_variable cv_;
        std::atomic<bool> shutdown_{false};
        std::vector<std::function<void()>> vector_;
    
    public:
        
        void push_back(std::function<void()> task)
        {
            {
            std::lock_guard<std::mutex>lock(mtx_);
            if (shutdown_)
            {
                return;
            }
            vector_.push_back(std::move(task));
            }
            cv_.notify_one();
        }

        std::optional<std::function<void()>> pop_back()
        {
            std::unique_lock<std::mutex>lock(mtx_);
            cv_.wait(lock,[this]
            {
                return !vector_.empty() || shutdown_;
            });
            if (vector_.empty())
                return std::nullopt;
            
            auto task =std::move(vector_.back());
            vector_.pop_back();
            return task;
        }

        std::optional<std::function<void()>> get(int index) const
        {
            std::lock_guard<std::mutex>lock(mtx_);
            if(index<0 || index >= vector_.size())
            {
                return std::nullopt;
            }
            return vector_[index];
            

        }
        void stop()
        {
            shutdown_=true;
            cv_.notify_all();
        }

        bool empty() const
        {
            std::lock_guard<std::mutex>lock(mtx_);
            return vector_.empty();
        }

        int size() const
        {
            std::lock_guard<std::mutex>lock(mtx_);
            return vector_.size();
        }
};

