/**
 * @file DynamicThreadPool.cpp
 * @brief 动态线程池实现
 *
 * Copyright (c) 2025 He, Pengpeng
 *
 * Licensed under the MIT License. See LICENSE file in the project root for details.
 */
#include "DynamicThreadPool.h"
#include <iostream>
#include <thread> // 用于获取硬件支持的最大线程数

DynamicThreadPool::DynamicThreadPool(size_t threadCount)
    : stop(false), maxQueueSize(0), taskThreshold(0), scalingFactor(0) {
  setThreadCount(threadCount);
}

DynamicThreadPool::~DynamicThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    stop = true;
  }
  condition.notify_all();
  for (std::thread &thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

void DynamicThreadPool::setThreadCount(size_t count) {
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    stop = true;
  }
  condition.notify_all();
  for (std::thread &thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  threads.clear();

  stop = false;
  for (size_t i = 0; i < count; ++i) {
    threads.emplace_back(&DynamicThreadPool::workerThread, this);
  }
}

void DynamicThreadPool::setMaxQueueSize(size_t maxSize) {
  std::unique_lock<std::mutex> lock(queueMutex);
  maxQueueSize = maxSize;
}

void DynamicThreadPool::setTaskThresholdAndScaling(size_t threshold,
                                                   size_t scalingFactor) {
  std::unique_lock<std::mutex> lock(queueMutex);
  taskThreshold = threshold;
  this->scalingFactor = scalingFactor;
}

void DynamicThreadPool::addTask(const std::string &name,
                                std::function<void(std::shared_ptr<void>)> func,
                                std::shared_ptr<void> arg) {
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    condition.wait(lock, [this] {
      return stop || maxQueueSize == 0 || taskQueue.size() < maxQueueSize;
    });
    if (stop) {
      return;
    }
    taskQueue.push({name, func, arg});

    if (taskThreshold > 0 && scalingFactor > 0 && taskQueue.size() > taskThreshold) {
      size_t maxThreads = std::thread::hardware_concurrency() / 2;
      size_t currentThreads = threads.size();
      size_t newThreads = std::min(scalingFactor, maxThreads > currentThreads ? maxThreads - currentThreads : 0);

      for (size_t i = 0; i < newThreads; ++i) {
        threads.emplace_back(&DynamicThreadPool::workerThread, this);
      }
      taskThreshold += scalingFactor;
    }
  }
  condition.notify_one();
}

void DynamicThreadPool::workerThread() {
  while (true) {
    Task task;
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      condition.wait(lock, [this] { return stop || !taskQueue.empty(); });
      if (stop && taskQueue.empty()) {
        return;
      }
      task = taskQueue.front();
      taskQueue.pop();
    }
    condition.notify_one();
    try {
      task.func(task.arg);
    } catch (const std::exception &e) {
      std::cerr << "Task " << task.name << " failed: " << e.what() << std::endl;
    }
  }
}
