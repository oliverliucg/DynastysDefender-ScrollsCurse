#pragma once
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

#include "alhelpers.h"
// #include "alhelpers.h"

enum FormatType { Int16, Float, IMA4, MSADPCM };

class SoundEngine {
 public:
  // Get the singleton instance
  static SoundEngine& GetInstance();
  ALuint LoadSound(const std::string& name, const std::string& filename);
  void PlaySound(const std::string& name);
  void StopSound(const std::string& name);

  void CleanUp();

 private:
  SoundEngine();
  // Delete copy constructor and assignment operator to prevent copying
  SoundEngine(const SoundEngine& other) = delete;
  SoundEngine& operator=(const SoundEngine& other) = delete;
  ~SoundEngine();
  ALCdevice* device_;
  ALCcontext* context_;
  std::unordered_map<std::string, ALuint> buffers_;  // Map of sound buffers
};