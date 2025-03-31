/**
 * @file main.cpp
 * @brief 示例程序，展示如何使用 DynamicThreadPool
 *
 * Copyright (c) 2025 He, Pengpeng
 *
 * Licensed under the MIT License. See LICENSE file in the project root for
 * details.
 */
#include "DynamicThreadPool.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

std::mutex logMutex; ///< 全局互斥锁，用于日志输出的同步

int main() {
  DynamicThreadPool pool(3); // 创建线程池，包含3个线程
  pool.setMaxQueueSize(10);  // 设置任务队列的最大大小为10
  pool.setTaskThresholdAndScaling(5, 2); // 当任务数量超过5时，每次增加2个线程

  for (int i = 0; i < 20; ++i) { // 尝试添加20个任务
    // 使用 shared_ptr<void> 管理任务参数，并指定自定义删除器
    std::shared_ptr<void> arg(new int(i + 1), [](void *p) {
      delete static_cast<int *>(p); // 自定义删除器，释放内存
    });

    pool.addTask(
        "Task" + std::to_string(i + 1),
        [](std::shared_ptr<void> arg) {
          auto num = *static_cast<int *>(arg.get()); // 获取参数
          {
            std::lock_guard<std::mutex> lock(logMutex);
            std::cout << "Task running with arg: " << num << " on thread "
                      << std::this_thread::get_id() << std::endl
                      << std::flush;
          }
          std::this_thread::sleep_for(std::chrono::seconds(1));
        },
        arg); // 将 shared_ptr<void> 传递给任务
  }

  // 不再显式等待，析构函数会处理线程的结束
  return 0;
}
