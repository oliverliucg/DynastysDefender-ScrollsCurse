#pragma once

#include <atomic>
#include <thread>
#include <unordered_map>

class ThreadHandler {
 public:
  // Get the singleton instance
  static ThreadHandler& GetInstance();

  // Create a new thread and return the thread id
  template <typename Function, typename... Args>
  std::thread::id CreateThread(Function&& f, Args&&... args) {
    std::thread newThread(std::forward<Function>(f),
                          std::forward<Args>(args)...);
    std::thread::id newId = newThread.get_id();
    thread_map_[newId] = std::move(newThread);
    thread_control_map_[newId].store(true, std::memory_order_relaxed);
    return newId;
  }

  // Terminates a thread by setting the flag to false and then detaching the
  // thread.
  void TerminateThread(std::thread::id id);

  // Joins a thread by setting the flag to false and then joining the thread.
  void JoinThread(std::thread::id id);

  // Gets a reference to the thread flag.
  std::atomic<bool>& GetThreadFlag(std::thread::id id) {
    return thread_control_map_.at(id);
  }

 private:
  // Constructor
  ThreadHandler() = default;

  // Destructor
  ~ThreadHandler() = default;

  // Copy constructor
  ThreadHandler(const ThreadHandler&) = delete;

  // Move constructor
  ThreadHandler(ThreadHandler&&) = delete;

  // Copy assignment operator
  ThreadHandler& operator=(const ThreadHandler&) = delete;

  // Move assignment operator
  ThreadHandler& operator=(ThreadHandler&&) = delete;

  // A map to store the thread id and the thread object
  std::unordered_map<std::thread::id, std::thread> thread_map_;

  // A map to store the thread id and the flag to terminate the thread
  std::unordered_map<std::thread::id, std::atomic<bool>> thread_control_map_;
};