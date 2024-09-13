
// threadpool.hpp

#pragma once

#include <algorithm>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

class ThreadPool {
 public:
  ThreadPool(std::size_t);
  template <typename Func, typename... Args>
  auto enqueue(Func&& func, Args&&... args);
  ~ThreadPool();

 private:
  std::vector<std::thread> thread_;
  std::queue<std::function<void()>> task_queue_;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool stop_;
};

inline ThreadPool::ThreadPool(size_t size) : stop_(false) {
  for (size_t i = 0; i < size; i++) {
    this->thread_.emplace_back([this] {
      for (;;) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock_guard_(this->mutex_);
          this->cv_.wait(
              lock_guard_, [this]() -> auto{
                return this->stop_ || !this->task_queue_.empty();
              });
          if (this->stop_ && this->task_queue_.empty()) {
            return;
          }
          task = std::move(this->task_queue_.front());
          this->task_queue_.pop();
        }
        task();
      }
    });
  }
}

template <typename Func, typename... Args>
auto ThreadPool::enqueue(Func&& func, Args&&... args) {
  using return_type = decltype(func(args...));
  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
  auto res = task->get_future();
  {
    std::unique_lock<std::mutex> lock_guard_(this->mutex_);
    this->task_queue_.emplace([task]() { task.operator*()(); });
  }
  this->cv_.notify_one();
  return res;
}