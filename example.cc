#include <chrono>
#include <iostream>
#include <ostream>
#include <thread>

#include "include/threadpool.hpp"

int print(int i) {
  std::cout << i;
  return i;
}

int main() {
  auto thread_pool = new ThreadPool(10);
  auto res = std::vector<std::future<int>>();
  for (int i = 0; i < 10; i++) {
    res.emplace_back(thread_pool->add_task(print, i));
  }
  for (auto&& r : res) {
    std::cout << r.get() << std::endl;
  }
}