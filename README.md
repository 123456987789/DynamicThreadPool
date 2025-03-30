# DynamicThreadPool - 动态线程池实现

## 项目简介

`DynamicThreadPool` 是一个动态线程池实现，支持以下功能：
- 动态调整线程数量，适应任务负载。
- 限制任务队列的最大大小，避免资源耗尽。
- 根据任务数量动态扩展线程池，确保高效执行。
- 线程池运行时不会影响计算机的正常运行。

项目包含一个示例程序，展示如何使用 `DynamicThreadPool` 执行多线程任务。

---

## 文件结构

- **DynamicThreadPool.h**: 动态线程池类的头文件，定义了线程池的接口。
- **DynamicThreadPool.cpp**: 动态线程池类的实现文件，包含线程池的具体逻辑。
- **main.cpp**: 示例程序，展示如何使用动态线程池。
- **Makefile**: 用于编译项目的 Makefile。
- **.vscode/tasks.json**: VS Code 的任务配置文件，用于快速构建项目。
- **.vscode/launch.json**: VS Code 的调试配置文件，用于调试项目。
- **.vscode/settings.json**: VS Code 的设置文件，优化 C++ 文件的语法高亮和 IntelliSense。

---

## 功能特性

### DynamicThreadPool 类

#### 构造函数
```cpp
DynamicThreadPool(size_t threadCount = 1);
```
- 初始化线程池并创建指定数量的线程。
- 默认创建 1 个线程。

#### 析构函数
```cpp
~DynamicThreadPool();
```
- 销毁线程池并等待所有线程退出。

#### 设置线程数量
```cpp
void setThreadCount(size_t count);
```
- 设置线程池中的线程数量。
- 该操作会停止当前线程池并重新创建线程。

#### 设置任务队列的最大大小
```cpp
void setMaxQueueSize(size_t maxSize);
```
- 设置任务队列的最大任务数量。
- 如果任务队列已满，`addTask` 方法会阻塞，直到可以添加任务。

#### 添加任务
```cpp
void addTask(const std::string &name, std::function<void(std::shared_ptr<void>)> func, std::shared_ptr<void> arg);
```
- 向线程池添加一个任务。
- 如果任务队列已满，方法会阻塞，直到可以添加任务。

#### 设置任务数量阈值和线程池扩展策略
```cpp
void setTaskThresholdAndScaling(size_t threshold, size_t scalingFactor);
```
- 设置任务数量阈值和线程池扩展策略。
- 当任务数量超过阈值时，线程池会动态扩展线程数量。
- 扩展的线程数量由 `scalingFactor` 决定，且不会超过硬件支持线程数的一半。

---

## 示例程序

`main.cpp` 展示了如何使用动态线程池执行多线程任务。

### 示例代码
```cpp
int main() {
    DynamicThreadPool pool(3);       // 创建线程池，包含3个线程
    pool.setMaxQueueSize(10);        // 设置任务队列的最大大小为10
    pool.setTaskThresholdAndScaling(5, 2); // 当任务数量超过5时，每次增加2个线程

    for (int i = 0; i < 20; ++i) { // 尝试添加20个任务
        std::shared_ptr<void> arg(new int(i + 1), [](void *p) {
            delete static_cast<int *>(p); // 自定义删除器，释放内存
        });

        pool.addTask(
            "Task" + std::to_string(i + 1),
            [](std::shared_ptr<void> arg) {
                auto num = *static_cast<int *>(arg.get());
                {
                    std::lock_guard<std::mutex> lock(logMutex);
                    std::cout << "Task running with arg: " << num << " on thread "
                              << std::this_thread::get_id() << std::endl
                              << std::flush;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            },
            arg);
    }

    return 0;
}
```

### 输出示例
程序运行时，输出类似以下内容：
```
Task running with arg: 1 on thread 140735532950272
Task running with arg: 2 on thread 140735541342976
Task running with arg: 3 on thread 140735549735680
Task running with arg: 4 on thread 140735532950272
Task running with arg: 5 on thread 140735541342976
...
```

---

## 编译与运行

### 使用 Makefile
1. 在终端中运行以下命令：
   ```bash
   make
   ```
2. 运行生成的可执行文件：
   ```bash
   ./main.exe
   ```

### 使用 VS Code
1. 打开 `tasks.json` 文件。
2. 按下 `Ctrl+Shift+B` 或选择 `Terminal -> Run Build Task`，运行构建任务。
3. 在终端中运行生成的可执行文件：
   ```bash
   ./main.exe
   ```

---

## 调试

### 使用 VS Code 调试
1. 打开 `launch.json` 文件。
2. 选择以下调试配置：
   - **Debug Project (Makefile)**: 调试整个项目。
   - **Debug Active File**: 调试当前活动文件。
3. 按下 `F5` 或选择 `Run -> Start Debugging` 开始调试。

---

## 注意事项

1. **线程池扩展限制**: 为避免影响计算机的正常运行，线程池的最大线程数被限制为硬件支持线程数的一半。
2. **任务队列限制**: 如果任务队列已满，`addTask` 方法会阻塞，直到可以添加任务。
3. **动态扩展**: 线程池会根据任务数量动态扩展线程，但不会超过设定的限制。
4. **线程安全**: 线程池的实现使用互斥锁和条件变量，确保任务队列的线程安全。

---

## 贡献

欢迎对本项目提出改进建议或提交代码贡献！您可以通过提交 Pull Request 或 Issue 的方式参与项目开发。

---

## 许可证

本项目基于 MIT 许可证开源，详情请参阅 LICENSE 文件。