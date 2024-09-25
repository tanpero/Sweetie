#ifndef _BENCHMARK_HPP_
#define _BENCHMARK_HPP_

/*
 * A minimal benchmark framework.
 *
 * Usage:
 *   BENCHMARK(your_function, iteration_times)
 *   BENCHMARK([](){ std::sin(1) * std::cos(1); }, 100)
 * 
 * Written by Camille.
 *   2023-9-14
 *
*/

#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <functional>
#include <numeric>
#include <iomanip>
#include <algorithm>

namespace __benchmark
{

    
// 定义基准测试函数的类型
using BenchmarkFunction = std::function<void()>;

#ifdef _WIN32  // Windows 平台

#include <windows.h>
#include <psapi.h>

// 函数：获取当前进程的内存使用量（字节）
size_t GetMemoryUsage() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.PrivateUsage;
}

#else  // 非 Windows 平台

#include <sys/resource.h>

// 函数：获取当前进程的内存使用量（字节）
size_t GetMemoryUsage() {
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
    return static_cast<size_t>(rusage.ru_maxrss) * 1024;  // 转换为字节
}

#endif

// 执行基准测试并返回运行时间（毫秒）和内存使用量（字节）
double RunBenchmark(const BenchmarkFunction& func, int num_iterations,
        double& min_time, double& max_time, double& std_dev, size_t& memory_usage) {
    std::vector<double> runtimes;

    for (int i = 0; i < num_iterations; ++i) {
        auto start_time = std::chrono::high_resolution_clock::now();
        func();  // 执行基准测试函数
        auto end_time = std::chrono::high_resolution_clock::now();

        double runtime_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        runtimes.push_back(runtime_ms);
    }

    // 计算最长时间、最短时间、平均时间和标准差
    min_time = *std::min_element(runtimes.begin(), runtimes.end());
    max_time = *std::max_element(runtimes.begin(), runtimes.end());
    double average_time = std::accumulate(
                runtimes.begin(), runtimes.end(), 0.0) / static_cast<double>(num_iterations);

    // 计算标准差
    double sum_squared_diff = 0.0;
    for (double runtime : runtimes) {
        sum_squared_diff += (runtime - average_time) * (runtime - average_time);
    }
    std_dev = std::sqrt(sum_squared_diff / static_cast<double>(num_iterations));

    // 获取内存使用量
    memory_usage = GetMemoryUsage();

    return average_time;
}

}

#define BENCHMARK(func, count)\
do \
{ \
    int num_iterations = count; \
    double min_runtime, max_runtime, std_dev; \
    size_t memory_usage; \
    double average_runtime = __benchmark::RunBenchmark(func, num_iterations, min_runtime, max_runtime, std_dev, memory_usage); \
    std::cout << "Average Runtime: " << average_runtime << " ms" << std::endl; \
    std::cout << "Minimum Runtime: " << min_runtime << " ms" << std::endl; \
    std::cout << "Maximum Runtime: " << max_runtime << " ms" << std::endl; \
    std::cout << "Standard Deviation of Runtime: " << std_dev << " ms" << std::endl; \
    std::cout << "Memory Usage: " << memory_usage << " bytes" << std::endl; \
} \
while (0)

#endif // !_BENCHMARK_HPP_


