#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <mutex>
#include <limits>

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


void clear_screen() {
    std::cout << "\033[2J\033[1;1H";
}

void print_separator() {
    std::cout << "─────────────────────────────────\n";
}

int get_choice(int min, int max) {
    int choice;
    while (true) {
        std::cout << "\nEnter choice (" << min << "-" << max << "): ";
        std::cin >> choice;
        if (std::cin.fail() || choice < min || choice > max) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid choice. Try again.\n";
        } else {
            std::cin.ignore();
            return choice;
        }
    }
}

std::string get_string(const std::string& prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

// ═══════════════════════════════════════
// MAIN MENU
// ═══════════════════════════════════════
void print_main_menu() {
    clear_screen();
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════╗\n";
    std::cout << "║   Thread Safe Containers Demo     ║\n";
    std::cout << "╠═══════════════════════════════════╣\n";
    std::cout << "║  Select a container:              ║\n";
    std::cout << "║                                   ║\n";
    std::cout << "║  1. ThreadSafeQueue  (FIFO)       ║\n";
    std::cout << "║  2. ThreadSafeStack  (LIFO)       ║\n";
    std::cout << "║  3. ThreadSafeVector (Index)      ║\n";
    std::cout << "║  4. ThreadSafeList   (Front/Back) ║\n";
    std::cout << "║  5. ThreadSafeMap    (Key-Value)  ║\n";
    std::cout << "║  6. Run ALL demos                 ║\n";
    std::cout << "║  0. Exit                          ║\n";
    std::cout << "╚═══════════════════════════════════╝\n";
}


void queue_menu() {
    ThreadSafeQueue q;
    std::vector<std::thread> workers;
    bool workers_running = false;

    while (true) {
        clear_screen();
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════╗\n";
        std::cout << "║       ThreadSafeQueue (FIFO)      ║\n";
        std::cout << "╠═══════════════════════════════════╣\n";
        std::cout << "║  1. Push a task                   ║\n";
        std::cout << "║  2. Push multiple tasks           ║\n";
        std::cout << "║  3. Start workers                 ║\n";
        std::cout << "║  4. Show size                     ║\n";
        std::cout << "║  5. Check if empty                ║\n";
        std::cout << "║  6. Stop and join workers         ║\n";
        std::cout << "║  0. Back to main menu             ║\n";
        std::cout << "╚═══════════════════════════════════╝\n";

        int choice = get_choice(0, 6);

        switch (choice) {
            case 1: {
                std::string msg = get_string("Enter task message: ");
                q.push([msg]{ safe_print("  [Queue] Running: " + msg); });
                std::cout << "Task pushed!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 2: {
                int n;
                std::cout << "How many tasks? ";
                std::cin >> n;
                std::cin.ignore();
                for (int i = 1; i <= n; i++) {
                    std::string msg = "task_" + std::to_string(i);
                    q.push([msg]{ safe_print("  [Queue] Running: " + msg); });
                }
                std::cout << n << " tasks pushed!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 3: {
                if (workers_running) {
                    std::cout << "Workers already running!\n";
                } else {
                    int n;
                    std::cout << "How many workers? ";
                    std::cin >> n;
                    std::cin.ignore();
                    for (int i = 1; i <= n; i++) {
                        workers.push_back(std::thread([&q, i]{
                            safe_print("  Worker " + std::to_string(i) + " started");
                            while (true) {
                                auto task = q.pop();
                                if (!task) {
                                    safe_print("  Worker " + std::to_string(i) + " exiting");
                                    break;
                                }
                                (*task)();
                            }
                        }));
                    }
                    workers_running = true;
                    std::cout << n << " workers started!\n";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 4: {
                std::cout << "Queue size: " << q.size() << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 5: {
                std::cout << "Queue is " << (q.empty() ? "EMPTY" : "NOT empty") << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 6: {
                q.stop();
                for (auto& w : workers) w.join();
                workers.clear();
                workers_running = false;
                std::cout << "All workers stopped!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 0: {
                if (workers_running) {
                    q.stop();
                    for (auto& w : workers) w.join();
                }
                return;
            }
        }
    }
}


void stack_menu() {
    ThreadSafeStack s;
    std::vector<std::thread> workers;
    bool workers_running = false;

    while (true) {
        clear_screen();
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════╗\n";
        std::cout << "║       ThreadSafeStack (LIFO)      ║\n";
        std::cout << "╠═══════════════════════════════════╣\n";
        std::cout << "║  1. Push a task                   ║\n";
        std::cout << "║  2. Push multiple tasks           ║\n";
        std::cout << "║  3. Start workers                 ║\n";
        std::cout << "║  4. Show size                     ║\n";
        std::cout << "║  5. Check if empty                ║\n";
        std::cout << "║  6. Stop and join workers         ║\n";
        std::cout << "║  0. Back to main menu             ║\n";
        std::cout << "╚═══════════════════════════════════╝\n";

        int choice = get_choice(0, 6);

        switch (choice) {
            case 1: {
                std::string msg = get_string("Enter task message: ");
                s.push([msg]{ safe_print("  [Stack] Running: " + msg); });
                std::cout << "Task pushed!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 2: {
                int n;
                std::cout << "How many tasks? ";
                std::cin >> n;
                std::cin.ignore();
                for (int i = 1; i <= n; i++) {
                    std::string msg = "task_" + std::to_string(i);
                    s.push([msg]{ safe_print("  [Stack] Running: " + msg); });
                }
                std::cout << n << " tasks pushed! (will run in reverse order)\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 3: {
                if (workers_running) {
                    std::cout << "Workers already running!\n";
                } else {
                    int n;
                    std::cout << "How many workers? ";
                    std::cin >> n;
                    std::cin.ignore();
                    for (int i = 1; i <= n; i++) {
                        workers.push_back(std::thread([&s, i]{
                            safe_print("  Worker " + std::to_string(i) + " started");
                            while (true) {
                                auto task = s.pop();
                                if (!task) {
                                    safe_print("  Worker " + std::to_string(i) + " exiting");
                                    break;
                                }
                                (*task)();
                            }
                        }));
                    }
                    workers_running = true;
                    std::cout << n << " workers started!\n";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 4: {
                std::cout << "Stack size: " << s.size() << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 5: {
                std::cout << "Stack is " << (s.empty() ? "EMPTY" : "NOT empty") << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 6: {
                s.stop();
                for (auto& w : workers) w.join();
                workers.clear();
                workers_running = false;
                std::cout << "All workers stopped!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 0: {
                if (workers_running) {
                    s.stop();
                    for (auto& w : workers) w.join();
                }
                return;
            }
        }
    }
}


void vector_menu() {
    ThreadSafeVector v;
    std::vector<std::thread> workers;
    bool workers_running = false;

    while (true) {
        clear_screen();
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════╗\n";
        std::cout << "║       ThreadSafeVector            ║\n";
        std::cout << "╠═══════════════════════════════════╣\n";
        std::cout << "║  1. Push a task                   ║\n";
        std::cout << "║  2. Push multiple tasks           ║\n";
        std::cout << "║  3. Get task by index             ║\n";
        std::cout << "║  4. Start workers                 ║\n";
        std::cout << "║  5. Show size                     ║\n";
        std::cout << "║  6. Check if empty                ║\n";
        std::cout << "║  7. Stop and join workers         ║\n";
        std::cout << "║  0. Back to main menu             ║\n";
        std::cout << "╚═══════════════════════════════════╝\n";

        int choice = get_choice(0, 7);

        switch (choice) {
            case 1: {
                std::string msg = get_string("Enter task message: ");
                v.push_back([]{ safe_print("  [Vector] task running"); });
                std::cout << "Task pushed!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 2: {
                int n;
                std::cout << "How many tasks? ";
                std::cin >> n;
                std::cin.ignore();
                for (int i = 1; i <= n; i++) {
                    std::string msg = "task_" + std::to_string(i);
                    v.push_back([msg]{ safe_print("  [Vector] Running: " + msg); });
                }
                std::cout << n << " tasks pushed!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 3: {
                int idx;
                std::cout << "Enter index: ";
                std::cin >> idx;
                std::cin.ignore();
                auto task = v.get(idx);
                if (task) {
                    std::cout << "Task found at index " << idx << "\n";
                } else {
                    std::cout << "No task at index " << idx << "\n";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 4: {
                if (workers_running) {
                    std::cout << "Workers already running!\n";
                } else {
                    int n;
                    std::cout << "How many workers? ";
                    std::cin >> n;
                    std::cin.ignore();
                    for (int i = 1; i <= n; i++) {
                        workers.push_back(std::thread([&v, i]{
                            safe_print("  Worker " + std::to_string(i) + " started");
                            while (true) {
                                auto task = v.pop_back();
                                if (!task) {
                                    safe_print("  Worker " + std::to_string(i) + " exiting");
                                    break;
                                }
                                (*task)();
                            }
                        }));
                    }
                    workers_running = true;
                    std::cout << n << " workers started!\n";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 5: {
                std::cout << "Vector size: " << v.size() << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 6: {
                std::cout << "Vector is " << (v.empty() ? "EMPTY" : "NOT empty") << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 7: {
                v.stop();
                for (auto& w : workers) w.join();
                workers.clear();
                workers_running = false;
                std::cout << "All workers stopped!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 0: {
                if (workers_running) {
                    v.stop();
                    for (auto& w : workers) w.join();
                }
                return;
            }
        }
    }
}


void list_menu() {
    ThreadSafeList l;
    std::vector<std::thread> workers;
    bool workers_running = false;

    while (true) {
        clear_screen();
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════╗\n";
        std::cout << "║       ThreadSafeList              ║\n";
        std::cout << "╠═══════════════════════════════════╣\n";
        std::cout << "║  1. Push to back (normal)         ║\n";
        std::cout << "║  2. Push to front (priority)      ║\n";
        std::cout << "║  3. Push multiple to back         ║\n";
        std::cout << "║  4. Peek at front                 ║\n";
        std::cout << "║  5. Start workers                 ║\n";
        std::cout << "║  6. Show size                     ║\n";
        std::cout << "║  7. Check if empty                ║\n";
        std::cout << "║  8. Stop and join workers         ║\n";
        std::cout << "║  0. Back to main menu             ║\n";
        std::cout << "╚═══════════════════════════════════╝\n";

        int choice = get_choice(0, 8);

        switch (choice) {
            case 1: {
                std::string msg = get_string("Enter task message: ");
                l.push_back([msg]{ safe_print("  [List] Running: " + msg); });
                std::cout << "Task pushed to back!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 2: {
                std::string msg = get_string("Enter priority task message: ");
                l.push_front([msg]{ safe_print("  [List PRIORITY] Running: " + msg); });
                std::cout << "Priority task pushed to front!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 3: {
                int n;
                std::cout << "How many tasks? ";
                std::cin >> n;
                std::cin.ignore();
                for (int i = 1; i <= n; i++) {
                    std::string msg = "task_" + std::to_string(i);
                    l.push_back([msg]{ safe_print("  [List] Running: " + msg); });
                }
                std::cout << n << " tasks pushed!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 4: {
                auto task = l.front();
                std::cout << "Front task " << (task ? "exists" : "does not exist") << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 5: {
                if (workers_running) {
                    std::cout << "Workers already running!\n";
                } else {
                    int n;
                    std::cout << "How many workers? ";
                    std::cin >> n;
                    std::cin.ignore();
                    for (int i = 1; i <= n; i++) {
                        workers.push_back(std::thread([&l, i]{
                            safe_print("  Worker " + std::to_string(i) + " started");
                            while (true) {
                                auto task = l.pop_front();
                                if (!task) {
                                    safe_print("  Worker " + std::to_string(i) + " exiting");
                                    break;
                                }
                                (*task)();
                            }
                        }));
                    }
                    workers_running = true;
                    std::cout << n << " workers started!\n";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
            }
            case 6: {
                std::cout << "List size: " << l.size() << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 7: {
                std::cout << "List is " << (l.empty() ? "EMPTY" : "NOT empty") << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 8: {
                l.stop();
                for (auto& w : workers) w.join();
                workers.clear();
                workers_running = false;
                std::cout << "All workers stopped!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 0: {
                if (workers_running) {
                    l.stop();
                    for (auto& w : workers) w.join();
                }
                return;
            }
        }
    }
}


void map_menu() {
    ThreadSafeMap m;

    while (true) {
        clear_screen();
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════╗\n";
        std::cout << "║       ThreadSafeMap               ║\n";
        std::cout << "╠═══════════════════════════════════╣\n";
        std::cout << "║  1. Set key value                 ║\n";
        std::cout << "║  2. Get value by key              ║\n";
        std::cout << "║  3. Remove key                    ║\n";
        std::cout << "║  4. Check if key exists           ║\n";
        std::cout << "║  5. Show size                     ║\n";
        std::cout << "║  6. Check if empty                ║\n";
        std::cout << "║  7. Clear all                     ║\n";
        std::cout << "║  0. Back to main menu             ║\n";
        std::cout << "╚═══════════════════════════════════╝\n";

        int choice = get_choice(0, 7);

        switch (choice) {
            case 1: {
                std::string key = get_string("Enter key: ");
                std::string val = get_string("Enter value: ");
                m.set(key, val);
                std::cout << "Set: " << key << " = " << val << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 2: {
                std::string key = get_string("Enter key: ");
                auto val = m.get(key);
                if (val) {
                    std::cout << key << " = " << *val << "\n";
                } else {
                    std::cout << key << " not found\n";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 3: {
                std::string key = get_string("Enter key to remove: ");
                m.remove(key);
                std::cout << key << " removed\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 4: {
                std::string key = get_string("Enter key to check: ");
                std::cout << key << (m.exists(key) ? " EXISTS" : " does NOT exist") << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 5: {
                std::cout << "Map size: " << m.size() << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 6: {
                std::cout << "Map is " << (m.empty() ? "EMPTY" : "NOT empty") << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 7: {
                m.clear();
                std::cout << "Map cleared!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(800));
                break;
            }
            case 0:
                return;
        }
    }
}


void run_all_demos() {
    safe_print("=== Running ALL Container Demos ===\n");

    
    {
        safe_print("\n--- ThreadSafeQueue (FIFO) ---");
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
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        q.stop();
        for (auto& w : workers) w.join();
    }

   
    {
        safe_print("\n--- ThreadSafeStack (LIFO) ---");
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
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        s.stop();
        for (auto& w : workers) w.join();
    }

    
    {
        safe_print("\n--- ThreadSafeVector ---");
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

    
    {
        safe_print("\n--- ThreadSafeList ---");
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
        l.push_back([]{ safe_print("  List task A normal"); });
        l.push_back([]{ safe_print("  List task B normal"); });
        l.push_front([]{ safe_print("  List task PRIORITY runs first!"); });
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        l.stop();
        for (auto& w : workers) w.join();
    }

    
    {
        safe_print("\n--- ThreadSafeMap ---");
        ThreadSafeMap m;
        m.set("name", "Joshva");
        m.set("language", "C++");
        m.set("project", "thread pool");
        auto name = m.get("name");
        if (name) safe_print("  name = " + *name);
        safe_print("  size = " + std::to_string(m.size()));
        m.remove("name");
        safe_print("  after remove, size = " + std::to_string(m.size()));
    }

    safe_print("\n=== All demos complete ===");
    std::this_thread::sleep_for(std::chrono::seconds(2));
}


int main() {
    while (true) {
        print_main_menu();
        int choice = get_choice(0, 6);

        switch (choice) {
            case 1: queue_menu();    break;
            case 2: stack_menu();    break;
            case 3: vector_menu();   break;
            case 4: list_menu();     break;
            case 5: map_menu();      break;
            case 6: run_all_demos(); break;
            case 0:
                std::cout << "\nGoodbye!\n";
                return 0;
        }
    }
}