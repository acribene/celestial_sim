#include "../headers/ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) 
    : m_stop(false), m_activeTasks(0), m_queuedTasks(0)
{
    // Create worker threads that will process tasks
    for (size_t i = 0; i < numThreads; ++i) {
        m_workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(m_queueMutex);
                    
                    // Wait for a task or stop signal
                    m_condition.wait(lock, [this] {
                        return m_stop || !m_tasks.empty();
                    });
                    
                    // Exit if we're stopping and no tasks left
                    if (m_stop && m_tasks.empty()) {
                        return;
                    }
                    
                    // Get task from queue
                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                    --m_queuedTasks;
                    ++m_activeTasks;
                }
                
                // Execute the task (outside the lock)
                task();
                
                // Mark task as complete
                {
                    std::lock_guard<std::mutex> lock(m_queueMutex);
                    --m_activeTasks;
                }
                m_waitCondition.notify_all();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_stop = true;
    }
    m_condition.notify_all();
    
    for (std::thread& worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_tasks.push(std::move(task));
        ++m_queuedTasks;
    }
    m_condition.notify_one();
}

void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_waitCondition.wait(lock, [this] {
        return m_queuedTasks == 0 && m_activeTasks == 0;
    });
}
