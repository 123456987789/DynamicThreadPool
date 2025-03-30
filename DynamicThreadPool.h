/**
 * @file DynamicThreadPool.h
 * @brief 动态线程池头文件
 *
 * Copyright (c) 2025 He, Pengpeng
 *
 * Licensed under the MIT License. See LICENSE file in the project root for details.
 */
#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

/**
 * @class DynamicThreadPool
 * @brief 一个动态线程池实现，用于管理和执行多线程任务。
 */
class DynamicThreadPool {
public:
  /**
   * @struct Task
   * @brief 表示线程池中的一个任务。
   */
  using Task = struct {
    std::string name;                                ///< 任务名称
    std::function<void(std::shared_ptr<void>)> func; ///< 任务函数
    std::shared_ptr<void> arg;                       ///< 任务函数的参数
  };

  /**
   * @brief 构造函数，创建指定数量的线程。
   * @param threadCount 线程池中的线程数量，默认为1。
   */
  DynamicThreadPool(size_t threadCount = 1);

  /**
   * @brief 析构函数，销毁线程池并等待所有线程退出。
   */
  ~DynamicThreadPool();

  /**
   * @brief 设置线程池中的线程数量。
   * @param count 新的线程数量。
   * @note 该操作会停止当前线程池并重新创建线程。
   */
  void setThreadCount(size_t count);

  /**
   * @brief 设置任务队列的最大大小。
   * @param maxSize 队列的最大任务数量。
   */
  void setMaxQueueSize(size_t maxSize);

  /**
   * @brief 向线程池添加一个任务。
   * @param name 任务名称。
   * @param func 任务函数。
   * @param arg 任务函数的参数。
   */
  void addTask(const std::string &name,
               std::function<void(std::shared_ptr<void>)> func,
               std::shared_ptr<void> arg);

  /**
   * @brief 设置任务数量阈值和线程池扩展策略。
   * @param threshold 任务数量阈值。
   * @param scalingFactor 每次扩展的线程数量。
   */
  void setTaskThresholdAndScaling(size_t threshold, size_t scalingFactor);

private:
  /**
   * @brief 工作线程的主循环，从任务队列中取出任务并执行。
   */
  void workerThread();

  std::vector<std::thread> threads;  ///< 线程池中的线程列表
  std::queue<Task> taskQueue;        ///< 任务队列
  std::mutex queueMutex;             ///< 任务队列的互斥锁
  std::condition_variable condition; ///< 用于线程同步的条件变量
  bool stop;                         ///< 标志线程池是否停止
  size_t maxQueueSize = 0; ///< 任务队列的最大大小，0 表示无限制
  size_t taskThreshold = 0; ///< 任务数量阈值，超过该值时扩展线程池
  size_t scalingFactor = 0; ///< 每次扩展的线程数量
};
