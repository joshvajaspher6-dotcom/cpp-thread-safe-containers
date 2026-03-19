# C++ Thread Safe Containers

A collection of 5 thread safe data structures built from scratch in C++17.
Each container is protected using the **monitor pattern**:
`mutex` + `condition_variable` + `atomic`.

Features an **interactive CLI menu** — select any container and operation at runtime.

---

## Containers

| Container | Internal Structure | Order | Use Case |
|---|---|---|---|
| `ThreadSafeQueue` | `std::queue` | FIFO | Thread pool tasks |
| `ThreadSafeStack` | `std::stack` | LIFO | Undo system |
| `ThreadSafeVector` | `std::vector` | Back to front | Shared list |
| `ThreadSafeList` | `std::list` | Front / Back | Priority tasks |
| `ThreadSafeMap` | `std::unordered_map` | Key Value | Shared cache |

---

## What Makes Them Thread Safe

Every container uses the same **monitor pattern**:
```cpp
mutable std::mutex              mtx_;
std::condition_variable         cv_;
std::atomic<bool>               shutdown_{false};
```

- `push()` locks mutex, adds task, notifies one waiting thread
- `pop()` sleeps efficiently until task arrives or shutdown signalled
- `stop()` sets shutdown flag and wakes all sleeping threads

---

## Interactive CLI Menu

Run the demo and select any container and operation interactively:
```
╔═══════════════════════════════════╗
║   Thread Safe Containers Demo     ║
╠═══════════════════════════════════╣
║  Select a container:              ║
║                                   ║
║  1. ThreadSafeQueue  (FIFO)       ║
║  2. ThreadSafeStack  (LIFO)       ║
║  3. ThreadSafeVector (Index)      ║
║  4. ThreadSafeList   (Front/Back) ║
║  5. ThreadSafeMap    (Key-Value)  ║
║  6. Run ALL demos                 ║
║  0. Exit                          ║
╚═══════════════════════════════════╝
```

### Queue Operations
```
1. Push a task
2. Push multiple tasks
3. Start workers
4. Show size
5. Check if empty
6. Stop and join workers
```

### Stack Operations
```
1. Push a task
2. Push multiple tasks
3. Start workers
4. Show size
5. Check if empty
6. Stop and join workers
```

### Vector Operations
```
1. Push a task
2. Push multiple tasks
3. Get task by index
4. Start workers
5. Show size
6. Check if empty
7. Stop and join workers
```

### List Operations
```
1. Push to back  (normal priority)
2. Push to front (high priority)
3. Push multiple to back
4. Peek at front
5. Start workers
6. Show size
7. Check if empty
8. Stop and join workers
```

### Map Operations
```
1. Set key value
2. Get value by key
3. Remove key
4. Check if key exists
5. Show size
6. Check if empty
7. Clear all
```

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

Select any container from the menu. Push tasks. Start workers. Watch them run.

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
Test project cpp-thread-safe-containers
    Start 1: QueueTest
1/5 Test #1: QueueTest  ........... Passed
    Start 2: StackTest
2/5 Test #2: StackTest  ........... Passed
    Start 3: VectorTest
3/5 Test #3: VectorTest ........... Passed
    Start 4: ListTest
4/5 Test #4: ListTest   ........... Passed
    Start 5: MapTest
5/5 Test #5: MapTest    ........... Passed

100% tests passed, 0 tests failed out of 5
```

---

## Container Behaviour
```
QUEUE  → push A B C → runs A B C    (first in first out)
STACK  → push A B C → runs C B A    (last in first out)
VECTOR → push A B C → runs C B A    (back to front)
LIST   → push_back A B, push_front P → runs P A B  (priority first)
MAP    → set/get/remove by key      (no task queue)
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
| `std::move` | Move tasks into container without copying |
| `std::function` | Store any callable as a task |

---

## Monitor Pattern

All containers follow the same pattern:
```
push():
  lock mutex
  add to container
  unlock mutex
  notify_one() — wake one worker

pop():
  lock mutex
  cv.wait() — sleep if empty
  take from container
  unlock mutex
  return task

stop():
  shutdown_ = true
  notify_all() — wake every worker
  workers see nullopt — exit
```

---

## Why I Built This

This project is the **learning foundation** before building a full
**C++ Thread Pool** from scratch.

Understanding how thread safe containers work internally is essential
for building production grade concurrent systems.

Each container taught a different lesson:
- **Queue** — the core thread pool task queue pattern
- **Stack** — LIFO scheduling and priority inversion
- **Vector** — index based access with concurrency
- **List** — push_front for priority task insertion
- **Map** — key value storage without task queuing

**Next project:** [cpp-thread-pool](https://github.com/joshvajasubre6-dotcom/cpp-thread-pool)

---

## Tech Stack

- Language: C++17
- Build: CMake 3.14+
- Testing: Google Test
- Threading: POSIX Threads (pthread)
- Platform: Linux

---

## Author

**Joshva** — building systems from scratch, one project at a time.
