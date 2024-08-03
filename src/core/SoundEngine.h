#pragma once
#include <assert.h>
#include <inttypes.h>
#include <iostream>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ResourceManager.h"
#include "StreamPlayer.h"
#include "ThreadHandler.h"

// Declare the external pointer variable
extern LPALCREOPENDEVICESOFT alcReopenDeviceSOFT;

enum FormatType { Int16, Float, IMA4, MSADPCM };

enum class StreamState { NOT_READY, READY, Playing, ENDED };

// Background music state
enum class BackgroundMusicState { Relaxing, Fighting, None };

struct BackgroundMusicInfo {
  BackgroundMusicState state{BackgroundMusicState::None};
  std::string currentMusic{};
  std::string lastMusic{};
  float timerInterval{0.f};
  const float expectedInterval{6.f};
  // background music (fighting)
  std::vector<std::string> fightingMusic;
  // background music (relaxing)
  std::vector<std::string> relaxingMusic;

  BackgroundMusicInfo() = default;

  void Refresh();

  void Reset();

  void StopAtEndOfCurrentMusic();
};

class SoundEngine {
 public:
  // Get the singleton instance
  static SoundEngine& GetInstance();

  ALuint LoadSound(const std::string& name, const std::string& filename,
                   float defaultVolume = 1.f);
  void UnloadSound(const std::string& name);
  void PlaySound(const std::string& name, bool loop = false,
                 float volume = -1.f);
  void StopSound(const std::string& name);

  // Streaming
  bool LoadStream(const std::string& name, const std::string& filename,
                  float defaultVolume = 1.f);
  void StreamPlayback(const std::string& streamName);
  void PlayStream(const std::string& name, float volume = -1.f);
  void ResetStream(const std::string& name);

  // Get count of times a sound has been played
  int GetPlayCount(const std::string& sourceName);

  // Gets the current playback position of a sound in seconds
  float GetPlaybackPosition(const std::string& sourceName);

  // Gets the remaining time of a sound in seconds
  float GetRemainingTime(const std::string& sourceName, float totalDuration);

  // Checks if a sound is playing
  bool IsPlaying(const std::string& sourceName);

  // Gets stream state
  StreamState GetStreamState(const std::string& sourceName);

  // Sets stream state
  void SetStreamState(const std::string& sourceName, StreamState state);

  // Checks if it's streaming
  bool IsStreamPlaying(const std::string& sourceName);

  // Checks if the stream ended
  bool IsStreamEnded(const std::string& sourceName);

  // Gradually change the volume of a sound
  void GraduallyChangeVolume(const std::string& sourceName, float targetVolume,
                             float duration);

  // Gradually change the volume of streams
  void GraduallyChangeStreamVolume(const std::string& sourceName,
                                   float targetVolume, float duration);

  // Checks if a sound is gradually changing volume
  bool IsGraduallyChangingVolume(const std::string& sourceName);

  // Checks if a stream is gradually changing volume
  bool IsGraduallyChangingStreamVolume(const std::string& sourceName);

  // Update the volume of a sound that is gradually changing
  void UpdateSourcesVolume(float dt);

  // Update the volume of streams that are gradually changing
  void UpdateStreamsVolume(float dt);

  // Sets the volume of a sound
  void SetVolume(const std::string& sourceName, float volume);

  // Gets the volume of a sound
  float GetVolume(const std::string& sourceName);

  void SetBackgroundMusicNames(const std::vector<std::string>& music,
                               bool isFighting = false);

  void StartBackgroundMusic(bool isFighting = false);

  void RefreshBackgroundMusic(float dt);

  void DoNotPlayNextBackgroundMusic();

  std::string GetPlayingBackgroundMusic() {
    return background_music_info_.currentMusic;
  }

  void Clear();

  // Clean up sources that are no longer playing. If force is true, clean up
  // all.
  void CleanUpSources(bool force = false);

  // Handle audio device change.
  void HandleAudioDeviceChange();

  // Update the sound engine.
  void Update(float dt);

 private:
  SoundEngine();
  // Delete copy constructor and assignment operator to prevent copying
  SoundEngine(const SoundEngine& other) = delete;
  SoundEngine& operator=(const SoundEngine& other) = delete;
  ~SoundEngine();

  ALCdevice* device_{nullptr};
  ALCcontext* context_{nullptr};
  std::string current_device_name_{""};
  bool is_cleared_{false};
  std::unordered_map<std::string, ALuint> buffers_;  // Map of sound buffers
  std::unordered_map<std::string, std::unique_ptr<StreamPlayer>>
      streams_;  // Map of streams
  std::unordered_map<std::string, StreamState>
      stream_states_;               // Map of stream states
  std::mutex stream_states_mutex_;  // Mutex for protecting stream_states_
  std::unordered_map<std::string, std::thread::id>
      stream_thread_ids_;  // Map of stream thread ids
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
  std::unordered_map<std::string,
                     glm::vec3>  // Map of gradually changing volumes
      gradually_changing_stream_volumes_;

  BackgroundMusicInfo background_music_info_;
  ALCint refresh_rate_{25};

  std::string GetFirstDeviceNameFromList();

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

  void SetBackgroundMusicState(BackgroundMusicState state) {
    background_music_info_.state = state;
  }
};