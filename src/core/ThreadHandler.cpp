/*
 * ThreadHandler.cpp
 * Copyright (C) 2024 Oliver Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "ThreadHandler.h"
#include <cassert>

ThreadHandler& ThreadHandler::GetInstance() {
  static ThreadHandler instance;
  return instance;
}

void ThreadHandler::TerminateThread(std::thread::id id) {
  // Safely update the control flag
  auto controlIt = thread_control_map_.find(id);
  assert(controlIt != thread_control_map_.end() &&
         "Thread does not exist in the control map.");
  controlIt->second.store(false, std::memory_order_relaxed);
  // Safely detach the thread
  auto threadIt = thread_map_.find(id);
  assert(threadIt != thread_map_.end() &&
         "Thread does not exist in the thread map.");
  if (threadIt->second.joinable()) {
    threadIt->second.detach();
    // Remove the thread from the map.
    thread_map_.erase(threadIt);
    // Remove the control flag from the map.
    thread_control_map_.erase(controlIt);
  }
}

void ThreadHandler::JoinThread(std::thread::id id) {
  // Safely update the control flag
  auto controlIt = thread_control_map_.find(id);
  assert(controlIt != thread_control_map_.end() &&
         "Thread does not exist in the control map.");
  controlIt->second.store(false, std::memory_order_relaxed);
  // Safely join the thread
  auto threadIt = thread_map_.find(id);
  assert(threadIt != thread_map_.end() &&
         "Thread does not exist in the thread map.");
  if (threadIt->second.joinable()) {
    threadIt->second.join();
    // Remove the thread from the map.
    thread_map_.erase(threadIt);
    // Remove the control flag from the map.
    thread_control_map_.erase(controlIt);
  }
}
