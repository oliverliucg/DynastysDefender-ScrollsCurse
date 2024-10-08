#pragma once
#include <GLFW/glfw3.h>

#include <cassert>
#include <string>
#include <unordered_map>
#include <utility>

class Timer {
 public:
  // Constructor
  Timer() = default;

  // Destructor
  ~Timer() = default;

  // Set timer for an event
  void SetEventTimer(std::string eventName, float time);

  // Start the timer for an event
  void StartEventTimer(std::string eventName);

  // pause the timer for an event
  void PauseEventTimer(std::string eventName);

  // resume the timer for an event
  void ResumeEventTimer(std::string eventName);

  // Check if the timer for an event has expired
  bool IsEventTimerExpired(std::string eventName);

  // clean an event
  void CleanEvent(std::string eventName);

  // Get event start time
  float GetEventStartTime(std::string eventName);

  // Get event used time
  float GetEventUsedTime(std::string eventName);

  // Get event timer
  float GetEventTimer(std::string eventName);

  // Get event remaining time
  float GetEventRemainingTime(std::string eventName);

  // Check if an event exists
  bool HasEvent(std::string eventName);

  // It's paused
  bool IsPaused(std::string eventName);

 private:
  std::unordered_map<std::string, float> eventTimers;
  std::unordered_map<std::string, float> usedTimes;
  std::unordered_map<std::string, float> startTimes;
};
