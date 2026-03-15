# Thread-Safe Distributed Lock Manager (DLM)

A high-performance C++ synchronization service designed to manage resource locks across an N-ary tree hierarchy. This project implements atomic locking mechanisms to prevent race conditions in highly concurrent environments.

## 📌 Project Overview
In large-scale systems, managing access to hierarchical resources (like file systems, organization charts, or database records) requires a locking mechanism that respects ancestor-descendant relationships. This DLM ensures that:
- A node cannot be locked if any ancestor is locked.
- A node cannot be locked if any descendant is locked.
- Simultaneous requests are handled safely via thread-safe synchronization.

## 🚀 Technical Highlights
* **Thread-Safety:** Implemented using `std::recursive_mutex` and `std::lock_guard` to ensure atomicity during hierarchical state updates.
* **RESTful API:** Developed a micro-server using `cpp-httplib` to expose `lock`, `unlock`, and `upgrade` endpoints.
* **Concurrency Validation:** Stress-tested with a multi-threaded Python suite firing **1,000+ simultaneous requests**, achieving 100% data integrity with zero deadlocks.
* **Optimized State Tracking:** Uses a `lock_descendant_count` approach to allow $O(h)$ time complexity for locking operations, where $h$ is the height of the tree.

## 🛠️ Tech Stack
- **Language:** C++17
- **Concurrency:** C++ Thread Support Library (std::thread, std::mutex)
- **Networking:** `cpp-httplib`
- **Compiler Flags:** `-D_WIN32_WINNT=0x0A00` (Windows 10+ support)
- **Testing:** Python 3.x (ThreadPoolExecutor)

## 📊 Performance Testing Results
Validated the locking logic by simulating a "thundering herd" of requests hitting the same resource:
- **Concurrent Threads:** 100
- **Total Requests:** 1,000
- **Successful Locks:** 1
- **Safely Rejected:** 999
- **Latency:** ~0.75s for total execution

## ⚙️ Setup & Execution
1. **Compile the server:**
   ```bash
   g++ LockManager.cpp -o server.exe -D_WIN32_WINNT=0x0A00 -lws2_32 -lpthread

   run the server
   ./server.exe
   Test via API:
    GET http://localhost:9999/lock?id=4&uid=1

## 🔮 Future Roadmap (Scaling to Production)
While the current implementation is robust, the following optimizations would scale it to a globally distributed system:

1. **Fine-Grained Locking:** Transition from a global recursive mutex to per-node mutexes to allow simultaneous operations on independent subtrees (increasing throughput by ~10x).
2. **Distributed Coordination:** Integrate **Redis** or **Zookeeper** to manage locks across multiple server instances instead of a single local process.
3. **Persistence Layer:** Add a **PostgreSQL** or **MongoDB** backend to recover the lock state in the event of a server crash.
4. **JWT Authentication:** Secure the API endpoints using JSON Web Tokens to ensure only authorized service accounts can acquire/release locks.