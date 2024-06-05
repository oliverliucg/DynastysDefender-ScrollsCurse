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
#include <string>e e
#include <unordered_map>
#include <unordered_set>
// #include "alhelpers.h"

enum FormatType { Int16, Float, IMA4, MSADPCM };

class SoundEngine {
 public:
  // Get the singleton instance
  static SoundEngine& GetInstance();
  ALuint LoadSound(const std::string& name, const std::string& filename);
  void PlaySound(const std::string& name, bool loop = false);
  void StopSound(const std::string& name);

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
  std::unordered_set<ALuint> sources_;               // Set of sound sources

  // Check if a source is looping
  bool IsLooping(ALuint source);
};