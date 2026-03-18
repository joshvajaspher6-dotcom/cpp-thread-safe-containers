#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <mutex>

#include "../include/thread_safe_queue.hpp"
#include "../include/thread_safe_stack.hpp"
#include "../include/thread_safe_vector.hpp"
#include "../include/thread_safe_list.hpp"
#include "../include/thread_safe_map.hpp"

std::mutex cout_mtx;

void safe_print(const std::string& msg) {
    std::lock_guard<std::mutex> lock(cout_mtx);
    std::cout << msg << "\n";
}


void demo_queue() {
    safe_print("\n=== ThreadSafeQueue (FIFO) ===");

    ThreadSafeQueue q;

    std::vector<std::thread> workers;
    for (int i = 1; i <= 2; i++)
        workers.push_back(std::thread([&q, i]{
            while (true) {
                auto task = q.pop();
                if (!task) break;
                (*task)();
            }
        }));

    q.push([]{ safe_print("  Queue task A"); });
    q.push([]{ safe_print("  Queue task B"); });
    q.push([]{ safe_print("  Queue task C"); });
    q.push([]{ safe_print("  Queue task D"); });

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    q.stop();
    for (auto& w : workers) w.join();
}


void demo_stack() {
    safe_print("\n=== ThreadSafeStack (LIFO) ===");

    ThreadSafeStack s;

    std::vector<std::thread> workers;
    for (int i = 1; i <= 2; i++)
        workers.push_back(std::thread([&s, i]{
            while (true) {
                auto task = s.pop();
                if (!task) break;
                (*task)();
            }
        }));

    s.push([]{ safe_print("  Stack task A"); });
    s.push([]{ safe_print("  Stack task B"); });
    s.push([]{ safe_print("  Stack task C"); });
    s.push([]{ safe_print("  Stack task D"); });

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    s.stop();
    for (auto& w : workers) w.join();
}


void demo_vector() {
    safe_print("\n=== ThreadSafeVector ===");

    ThreadSafeVector v;

    std::vector<std::thread> workers;
    for (int i = 1; i <= 2; i++)
        workers.push_back(std::thread([&v, i]{
            while (true) {
                auto task = v.pop_back();
                if (!task) break;
                (*task)();
            }
        }));

    v.push_back([]{ safe_print("  Vector task A"); });
    v.push_back([]{ safe_print("  Vector task B"); });
    v.push_back([]{ safe_print("  Vector task C"); });

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    v.stop();
    for (auto& w : workers) w.join();
}


void demo_list() {
    safe_print("\n=== ThreadSafeList ===");

    ThreadSafeList l;

    std::vector<std::thread> workers;
    for (int i = 1; i <= 2; i++)
        workers.push_back(std::thread([&l, i]{
            while (true) {
                auto task = l.pop_front();
                if (!task) break;
                (*task)();
            }
        }));

    l.push_back([]{ safe_print("  List task A — normal"); });
    l.push_back([]{ safe_print("  List task B — normal"); });
    l.push_front([]{ safe_print("  List task PRIORITY — runs first!"); });

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    l.stop();
    for (auto& w : workers) w.join();
}


void demo_map() {
    safe_print("\n=== ThreadSafeMap ===");

    ThreadSafeMap m;

    
    m.set("name",     "Joshva");
    m.set("age",      "20");
    m.set("language", "C++");
    m.set("project",  "thread pool");

  
    auto name = m.get("name");
    if (name) safe_print("  name = " + *name);

    auto age = m.get("age");
    if (age) safe_print("  age = " + *age);

    auto lang = m.get("language");
    if (lang) safe_print("  language = " + *lang);

    
    safe_print("  exists name? " +
        std::string(m.exists("name") ? "yes" : "no"));
    safe_print("  exists xyz?  " +
        std::string(m.exists("xyz")  ? "yes" : "no"));

   
    safe_print("  size = " + std::to_string(m.size()));

    
    m.remove("age");
    safe_print("  after remove age, size = " +
        std::to_string(m.size()));

   
    auto removed = m.get("age");
    safe_print("  get age after remove = " +
        std::string(removed ? *removed : "nullopt"));

   
    m.clear();
    safe_print("  after clear, size = " +
        std::to_string(m.size()));
}


int main() {
    safe_print("=== Thread Safe Containers Demo ===");

    demo_queue();
    demo_stack();
    demo_vector();
    demo_list();
    demo_map();

    safe_print("\n=== All demos complete ===");
    return 0;
}