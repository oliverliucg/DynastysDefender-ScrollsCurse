#pragma once
#include "alhelpers.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <assert.h>
#include <inttypes.h>
#include <iostream>
#include <limits.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ResourceManager.h"

enum FormatType { Int16, Float, IMA4, MSADPCM };

class SoundEngine {
 public:
  // Get the singleton instance
  static SoundEngine& GetInstance();
  ALuint LoadSound(const std::string& name, const std::string& filename,
                   float defaultVolume = 1.0f);
  void PlaySound(const std::string& name, bool loop = false,
                 float volume = -1.f);
  void StopSound(const std::string& name);

  // Get count of times a sound has been played
  int GetPlayCount(const std::string& sourceName);

  // Gets the current playback position of a sound in seconds
  float GetPlaybackPosition(const std::string& sourceName);

  // Gets the remaining time of a sound in seconds
  float GetRemainingTime(const std::string& sourceName, float totalDuration);

  // Checks if a sound is playing
  bool IsPlaying(const std::string& sourceName);

  // Gradually change the volume of a sound
  void GraduallyChangeVolume(const std::string& sourceName, float targetVolume,
                             float duration);

  // Checks if a sound is gradually changing volume
  bool IsGraduallyChangingVolume(const std::string& sourceName);

  // Update the volume of a sound that is gradually changing
  void UpdateSourcesVolume(float dt);

  // Sets the volume of a sound
  void SetVolume(const std::string& sourceName, float volume);

  // Gets the volume of a sound
  float GetVolume(const std::string& sourceName);

  void Clear();

  // Clean up sources that are no longer playing. If force is true, clean up
  // all.
  void CleanUpSources(bool force = false);

 private:
  SoundEngine();
  // Delete copy constructor and assignment operator to prevent copying
  SoundEngine(const SoundEngine& other) = delete;
  SoundEngine& operator=(const SoundEngine& other) = delete;
  ~SoundEngine();

  ALCdevice* device_;
  ALCcontext* context_;
  std::unordered_map<std::string, ALuint> buffers_;  // Map of sound buffers
  std::unordered_map<std::string, float> default_volumes_;  // Map of default
                                                            // sound volumes
  std::unordered_map<std::string, std::vector<ALuint>>
      sources_;                                       // Map of sound sources
  std::unordered_map<std::string, float> volumes_;    // Map of sound volumes
  std::unordered_map<std::string, int> play_counts_;  // Map of play counts
  std::unordered_map<std::string,
                     std::unordered_map<ALuint, glm::vec3>>  // Map of gradually
                                                             // changing volumes
      gradually_changing_volumes_;

  // Checks if a source is looping
  bool IsLooping(ALuint source);

  // Checks if a source is playing
  bool IsPlaying(ALuint source);

  // Sets the volume of a source
  void SetVolume(ALuint source, float volume);

  // Gets the volume of a source
  float GetVolume(ALuint source);

  // Gets the current playback position of a sound in seconds
  float GetPlaybackPosition(ALuint source);

  // Gets the remaining time of a sound in seconds
  float GetRemainingTime(ALuint source, float totalDuration);
};