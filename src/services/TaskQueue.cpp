#include "TaskQueue.h"

TaskQueue::TaskQueue(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&TaskQueue::workerThread, this);
    }
}

TaskQueue::~TaskQueue() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    
    for (std::thread& worker : workers) {
        worker.join();
    }
}

void TaskQueue::workerThread() {
    while (true) {
        std::function<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });
            
            if (stop && tasks.empty()) {
                return;
            }
            
            task = std::move(tasks.front());
            tasks.pop();
        }
        
        task();
    }
}