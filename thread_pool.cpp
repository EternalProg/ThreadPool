#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <thread_pool.hpp>
#include <vector>

namespace shttp {

ThreadPool::ThreadPool(uint16_t thread_count) : stop_(false) {
  for (size_t i = 0; i < thread_count; ++i) {
    workers_.emplace_back([this]() {
      while (true) {
        Task task;

        {
          std::unique_lock<std::mutex> lock(mutex_);
          cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

          if (stop_ && tasks_.empty()) return;

          task = std::move(tasks_.front());
          tasks_.pop();
        }

        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    stop_ = true;
  }
  cv_.notify_all();
  for (auto& t : workers_) t.join();
}

void ThreadPool::submit(Task task) {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.push(std::move(task));
  }
  cv_.notify_one();
}

}  // namespace shttp