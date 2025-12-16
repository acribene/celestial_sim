#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();
    
    // Submit a task to the pool
    void enqueue(std::function<void()> task);
    
    // Wait for all tasks to complete
    void wait();
    
    // Prevent copying
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;
    
private:
    std::vector<std::thread> m_workers; // Worker threads
    std::queue<std::function<void()>> m_tasks; // Task queue
    
    std::mutex m_queueMutex; // Mutex for task queue
    std::condition_variable m_condition; // Condition variable for task availability
    std::condition_variable m_waitCondition; // Condition variable for wait() method
    
    std::atomic<bool> m_stop; // Atomic flag to stop the pool
    std::atomic<size_t> m_activeTasks; // Atomic count of active tasks
    std::atomic<size_t> m_queuedTasks; // Atomic count of queued tasks
};

#endif // THREADPOOL_H
