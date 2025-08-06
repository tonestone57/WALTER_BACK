/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    void Enqueue(std::function<void()> task);

private:
    std::vector<std::thread> fWorkers;
    std::queue<std::function<void()>> fTasks;

    std::mutex fQueueMutex;
    std::condition_variable fCondition;
    bool fStop;
};

#endif // THREAD_POOL_H
