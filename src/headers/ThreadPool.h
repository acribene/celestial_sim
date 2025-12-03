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
    ThreadPool& operator=(const ThreadPool&) = delete;
    
private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    std::condition_variable m_waitCondition;
    
    std::atomic<bool> m_stop;
    std::atomic<size_t> m_activeTasks;
    std::atomic<size_t> m_queuedTasks;
};

#endif // THREADPOOL_H
