#ifndef SHTTP_THREAD_POOL_HPP
#define SHTTP_THREAD_POOL_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace shttp {

class ThreadPool {
 public:
  using Task = std::function<void()>;

  ThreadPool() = default;

  explicit ThreadPool(uint16_t thread_count);

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  void submit(Task task);

  ~ThreadPool();

 private:
  std::vector<std::thread> workers_;
  std::queue<Task> tasks_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::atomic<bool> stop_;
};

}  // namespace shttp

#endif