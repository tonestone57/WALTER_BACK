/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "rendering/ThreadPool.h"

ThreadPool::ThreadPool(size_t threads)
    :
    fStop(false)
{
    for(size_t i = 0; i < threads; ++i)
        fWorkers.emplace_back(
            [this]
            {
                for(;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->fQueueMutex);
                        this->fCondition.wait(lock,
                            [this]{ return this->fStop || !this->fTasks.empty(); });
                        if(this->fStop && this->fTasks.empty())
                            return;
                        task = std::move(this->fTasks.front());
                        this->fTasks.pop();
                    }
                    task();
                }
            }
        );
}

void
ThreadPool::Enqueue(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(fQueueMutex);
        if(fStop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        fTasks.emplace(task);
    }
    fCondition.notify_one();
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(fQueueMutex);
        fStop = true;
    }
    fCondition.notify_all();
    for(std::thread &worker: fWorkers)
        worker.join();
}
