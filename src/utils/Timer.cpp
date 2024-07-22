/*
 * Timer.cpp
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

#include "Timer.h"

#include <iostream>

Timer::Timer() {}

Timer::~Timer() {}

void Timer::SetEventTimer(std::string eventName, float time) {
  eventTimers[eventName] = time;
}

void Timer::StartEventTimer(std::string eventName) {
  assert(eventTimers.find(eventName) != eventTimers.end() &&
         "Event timer not found");
  startTimes[eventName] = glfwGetTime();
  usedTimes[eventName] = 0.0f;
}

void Timer::PauseEventTimer(std::string eventName) {
  assert(startTimes.find(eventName) != startTimes.end() &&
         "Event start time not found");
  assert(startTimes[eventName] >= 0.f && "Event timer is already paused");
  usedTimes[eventName] += glfwGetTime() - startTimes[eventName];
  startTimes[eventName] =
      -1.0f;  // -1.0f means the timer for this event is paused.
}

void Timer::ResumeEventTimer(std::string eventName) {
  assert(startTimes.find(eventName) != startTimes.end() &&
         "Event start time not found");
  assert(startTimes[eventName] < 0.f && "Event timer is not paused");
  assert(usedTimes.find(eventName) != usedTimes.end() &&
         "Event used time not found");
  startTimes[eventName] = glfwGetTime();
}

bool Timer::IsEventTimerExpired(std::string eventName) {
  assert(eventTimers.find(eventName) != eventTimers.end() &&
         "Event timer not found");
  assert(startTimes.find(eventName) != startTimes.end() &&
         "Event start time not found");
  assert(usedTimes.find(eventName) != usedTimes.end() &&
         "Event used time not found");
  assert(startTimes[eventName] >= 0.f && "Event timer is paused");
  if (glfwGetTime() - startTimes[eventName] + usedTimes[eventName] >
      eventTimers[eventName]) {
    return true;
  }
  return false;
}

float Timer::GetEventStartTime(std::string eventName) {
  assert(startTimes.find(eventName) != startTimes.end() &&
         "Event timer not found");
  return startTimes[eventName];
}

float Timer::GetEventUsedTime(std::string eventName) {
  assert(startTimes.find(eventName) != startTimes.end() &&
         "Event start time not found");
  assert(usedTimes.find(eventName) != usedTimes.end() &&
         "Event used time not found");
  return usedTimes[eventName] + glfwGetTime() - startTimes[eventName];
}

float Timer::GetEventTimer(std::string eventName) {
  assert(eventTimers.find(eventName) != eventTimers.end() &&
         "Event timer not found");
  return eventTimers[eventName];
}

float Timer::GetEventRemainingTime(std::string eventName) {
  assert(eventTimers.find(eventName) != eventTimers.end() &&
         "Event timer not found");
  assert(startTimes.find(eventName) != startTimes.end() &&
         "Event start time not found");
  assert(usedTimes.find(eventName) != usedTimes.end() &&
         "Event used time not found");
  assert(startTimes[eventName] >= 0.f && "Event timer is paused");
  return eventTimers[eventName] -
         (glfwGetTime() - startTimes[eventName] + usedTimes[eventName]);
}

void Timer::CleanEvent(std::string eventName) {
  eventTimers.erase(eventName);
  startTimes.erase(eventName);
  usedTimes.erase(eventName);
}

bool Timer::HasEvent(std::string eventName) {
  return eventTimers.find(eventName) != eventTimers.end();
}

bool Timer::IsPaused(std::string eventName) {
  assert(startTimes.find(eventName) != startTimes.end() &&
         "Event start time not found");
  return startTimes[eventName] < 0.f;
}