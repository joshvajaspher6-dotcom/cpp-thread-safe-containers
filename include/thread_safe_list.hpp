#pragma once

#include <atomic>
#include <list>
#include <mutex>
#include <optional>
#include <functional>
#include <condition_variable>

class ThreadSafeList
{
    private:
        mutable std::mutex mtx_;
        std::condition_variable cv_;
        std::list<std::function<void()>> list_;
        std::atomic<bool> shutdown_{false};

    public:

        void push_back(std::function<void()> task)
        {
            {
            std::lock_guard<std::mutex>lock(mtx_);
            if (shutdown_){
                return;
            }
            list_.push_back(std::move(task));

            }
            cv_.notify_one();

        }

         void push_front(std::function<void()> task)
        {
            {
            std::lock_guard<std::mutex>lock(mtx_);
            if (shutdown_){
                return;
            }
            list_.push_front(std::move(task));

            }
            cv_.notify_one();

        }
        std::optional<std::function<void()>> front()
        {
            std::unique_lock<std::mutex>lock(mtx_);
           

            if (list_.empty())
            {
                return std::nullopt;
            }
            return list_.front();
        }

        std::optional<std::function<void()>> pop_front()
        {
            std::unique_lock<std::mutex>lock(mtx_);

            cv_.wait(lock,[this]
            {
                return !list_.empty() || shutdown_; 
            });

            if (list_.empty())
            {
                return std::nullopt;
            }
            auto task = std::move(list_.front());

            list_.pop_front();

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
            return list_.empty();
        }

        int size() const
        {
            std::lock_guard<std::mutex>lock(mtx_);
            return list_.size();
        }

};