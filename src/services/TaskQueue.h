#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

class TaskQueue {
private:
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::vector<std::thread> workers;
    bool stop;
    
public:
    TaskQueue(size_t numThreads);
    ~TaskQueue();
    
    template<class F>
    void enqueue(F&& task);
    
private:
    void workerThread();
};

template<class F>
void TaskQueue::enqueue(F&& task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(std::forward<F>(task));
    }
    condition.notify_one();
}

#endif // TASK_QUEUE_H