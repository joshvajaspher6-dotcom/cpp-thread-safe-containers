# C++ Thread Safe Containers

A collection of 5 thread safe data structures built from scratch in C++17.
Each container is protected using the **monitor pattern**:
`mutex` + `condition_variable` + `atomic`.

---

## Containers

| Container | Internal Structure | Order | Use Case |
|---|---|---|---|
| `ThreadSafeQueue` | `std::queue` | FIFO | Thread pool tasks |
| `ThreadSafeStack` | `std::stack` | LIFO | Undo system |
| `ThreadSafeVector` | `std::vector` | Index access | Shared list |
| `ThreadSafeList` | `std::list` | Front / Back | Priority tasks |
| `ThreadSafeMap` | `std::unordered_map` | Key → Value | Shared cache |

---

## What Makes Them Thread Safe

Every container uses the same **monitor pattern**:
```cpp
std::mutex              mtx_;      // protects shared data
std::condition_variable cv_;       // efficient sleeping and waking
std::atomic<bool>       shutdown_; // fast shutdown flag
```

- `push()` locks the mutex, adds the task, notifies one waiting thread
- `pop()` sleeps efficiently until a task arrives or shutdown is signalled
- `stop()` sets shutdown flag and wakes all sleeping threads

---

## Project Structure
```
cpp-thread-safe-containers/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── thread_safe_queue.hpp
│   ├── thread_safe_stack.hpp
│   ├── thread_safe_vector.hpp
│   ├── thread_safe_list.hpp
│   └── thread_safe_map.hpp
├── src/
│   └── main.cpp
└── tests/
    ├── test_queue.cpp
    ├── test_stack.cpp
    ├── test_vector.cpp
    ├── test_list.cpp
    └── test_map.cpp
```

---

## How to Build
```bash
git clone https://github.com/joshvajasubre6-dotcom/cpp-thread-safe-containers
cd cpp-thread-safe-containers
mkdir build && cd build
cmake ..
cmake --build .
```

---

## How to Run Demo
```bash
./demo
```

Expected output:
```
=== Thread Safe Containers Demo ===

=== ThreadSafeQueue (FIFO) ===
  Queue task A
  Queue task B
  Queue task C
  Queue task D

=== ThreadSafeStack (LIFO) ===
  Stack task D
  Stack task C
  Stack task B
  Stack task A

=== ThreadSafeVector ===
  Vector task C
  Vector task B
  Vector task A

=== ThreadSafeList ===
  List task PRIORITY — runs first!
  List task A — normal
  List task B — normal

=== ThreadSafeMap ===
  name = Joshva
  age = 20
  language = C++
  exists name? yes
  exists xyz?  no
  size = 4
  after remove age, size = 3
  get age after remove = nullopt
  after clear, size = 0

=== All demos complete ===
```

---

## How to Run Tests
```bash
cd build
ctest --output-on-failure
```

Or run individually:
```bash
./test_queue
./test_stack
./test_vector
./test_list
./test_map
```

Expected:
```
100% tests passed, 0 tests failed
```

---

## Container Differences
```
QUEUE  → first in first out  → oldest task runs first
STACK  → last in first out   → newest task runs first
VECTOR → back to front       → index access available
LIST   → front to back       → push_front for priority tasks
MAP    → key value pairs     → no tasks, stores data
```

---

## Key Concepts Used

| Concept | Purpose |
|---|---|
| `std::mutex` | Protects shared data from concurrent access |
| `std::lock_guard` | Automatically locks and unlocks mutex |
| `std::unique_lock` | Flexible lock required for condition variable |
| `std::condition_variable` | Efficient thread sleeping and waking |
| `std::atomic<bool>` | Fast thread safe shutdown flag |
| `std::optional` | Return value that might be empty |
| `std::move` | Move tasks into queue without copying |
| `std::function` | Store any callable as a task |

---

## Why I Built This

This project is the learning foundation before building a full
**C++ Thread Pool** from scratch.

Understanding how thread safe containers work internally is essential
for building production grade concurrent systems.

**Next project:** [cpp-thread-pool](https://github.com/joshvajasubre6-dotcom/cpp-thread-pool)

---

## Tech Stack

- Language: C++17
- Build: CMake 3.14+
- Testing: Google Test
- Threading: POSIX Threads (pthread)

---

