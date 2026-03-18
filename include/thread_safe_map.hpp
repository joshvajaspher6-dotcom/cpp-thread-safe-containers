#pragma once

#include <unordered_map>
#include <mutex>
#include <optional>
#include <string>

class ThreadSafeMap {
private:
    mutable std::mutex mtx_;
    std::unordered_map<std::string, std::string> map_;

public:
    
    void set(std::string key,std::string value)
    {
        {
            std::lock_guard<std::mutex>lock(mtx_);
            map_[key]=std::move(value);
        }
    }

    std::optional<std::string> get(std::string key) const
    {
        {
            std::lock_guard<std::mutex>lock(mtx_);
            auto it = map_.find(key);
            if(it == map_.end())
            {
                return std::nullopt;
            }
            return it->second;
        }
    }

    void remove(std::string key)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        map_.erase(key);
    }

    bool exists(std::string key) const
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return map_.count(key) > 0;
    }

    int size() const
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return map_.size();
    }

    void clear()
    {   
        std::lock_guard<std::mutex> lock(mtx_);
        map_.clear();
    }  
    
    bool empty() const
        {
            std::lock_guard<std::mutex>lock(mtx_);
            return map_.empty();
        }

       
};