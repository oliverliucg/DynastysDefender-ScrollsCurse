/*
 * SoundEngine.cpp
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

#include "SoundEngine.h"

#include <alspan.h>
#include <alstring.h>

LPALBUFFERCALLBACKSOFT alBufferCallbackSOFT = nullptr;

void BackgroundMusicInfo::Refresh() {
  if (state == BackgroundMusicState::None) {
    return;
  }
  const auto& targetMusicVec =
      (state == BackgroundMusicState::Fighting) ? fightingMusic : relaxingMusic;
  assert(!targetMusicVec.empty() && "No music available");
  size_t nextMusicIdx = generateRandomInt<size_t>(0, targetMusicVec.size() - 1);
  while ((targetMusicVec[nextMusicIdx] == currentMusic ||
          targetMusicVec[nextMusicIdx] == lastMusic)) {
    if (targetMusicVec.size() <= 1) {
      break;
    } else if (targetMusicVec.size() == 2) {
      if (targetMusicVec[nextMusicIdx] == currentMusic) {
        nextMusicIdx = (nextMusicIdx + 1) % targetMusicVec.size();
        break;
      }
    } else {
      nextMusicIdx = (nextMusicIdx + 1) % targetMusicVec.size();
    }
  }
  lastMusic = currentMusic;
  currentMusic = targetMusicVec[nextMusicIdx];
  timerInterval = 0.f;
}

void BackgroundMusicInfo::Reset() {
  state = BackgroundMusicState::None;
  currentMusic = "";
  lastMusic = "";
  timerInterval = 0.0f;
}

void BackgroundMusicInfo::StopAtEndOfCurrentMusic() {
  state = BackgroundMusicState::None;
  timerInterval = 0.0f;
}

// Get the singleton instance
SoundEngine& SoundEngine::GetInstance() {
  static SoundEngine instance;
  return instance;
}

SoundEngine::SoundEngine() {
  device_ = alcOpenDevice(nullptr);
  if (device_ == nullptr) {
    std::cerr << "Error opening OpenAL device" << std::endl;
  }
  context_ = alcCreateContext(device_, nullptr);
  if (context_ == nullptr) {
    std::cerr << "Error creating OpenAL context" << std::endl;
  }
  if (!alcMakeContextCurrent(context_)) {
    std::cerr << "Error making OpenAL context current" << std::endl;
  }

  if (!alIsExtensionPresent("AL_SOFT_callback_buffer")) {
    std::cerr << "AL_SOFT_callback_buffer extension not available" << std::endl;
  }

  alBufferCallbackSOFT = reinterpret_cast<LPALBUFFERCALLBACKSOFT>(
      alGetProcAddress("alBufferCallbackSOFT"));

  alcGetIntegerv(alcGetContextsDevice(alcGetCurrentContext()), ALC_REFRESH, 1,
                 &refresh_rate_);

  /* alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);*/
  // Error checking function
  auto checkAlError = [](const std::string& filename, int line) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
      std::cerr << "OpenAL error in " << filename << " at line " << line << ": "
                << alGetString(error) << std::endl;
    }
  };
#define CHECK_AL_ERROR checkAlError(__FILE__, __LINE__)
}

SoundEngine::~SoundEngine() { Clear(); }

void SoundEngine::CleanUpSources(bool force) {
  std::vector<std::string> sourceNamesToDelete;
  std::unordered_set<ALuint> sourcesVisited;
  for (auto& source :
       sources_) {  // Assuming activeSources is a list of source IDs
    ALint state;
    std::unordered_set<ALint> sourcesToDelete;
    for (const auto& sourceID : source.second) {
      assert(sourcesVisited.count(sourceID) == 0 &&
             "Source visited more than once");
      sourcesVisited.insert(sourceID);
      alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
      // Check if the source is not playing or if force cleanup is enabled
      if (force || (state != AL_PLAYING && !IsLooping(sourceID))) {
        alDeleteSources(1, &sourceID);
        sourcesToDelete.insert(sourceID);
      }
    }
    if (sourcesToDelete.size() == source.second.size()) {
      sourceNamesToDelete.emplace_back(source.first);
    } else {
      // only keep the sources that are still playing
      std::vector<ALuint> remainingSources;
      for (const auto& sourceID : source.second) {
        if (sourcesToDelete.count(sourceID) == 0) {
          remainingSources.emplace_back(sourceID);
        }
      }
      source.second = remainingSources;
    }
  }
  // Remove the deleted sources from the active list
  for (auto sourceName : sourceNamesToDelete) {
    sources_.erase(sourceName);
  }
}

void SoundEngine::Clear() {
  // Stop and reset all streams.
  for (auto& [sourceName, stream] : streams_) {
    ResetStream(sourceName);
  }

  CleanUpSources(/*force=*/true);
  for (auto& buffer : buffers_) {
    alDeleteBuffers(1, &buffer.second);
  }
  // Join all threads
  for (auto& [name, thread] : stream_threads_) {
    if (thread.joinable()) {
      thread.join();  // Ensure the thread finishes
    }
  }
  // Clear stream objects
  streams_.clear();

  alcMakeContextCurrent(nullptr);
  alcDestroyContext(context_);
  alcCloseDevice(device_);
}

ALuint SoundEngine::LoadSound(const std::string& name,
                              const std::string& filename,
                              float defaultVolume) {
  enum FormatType sample_format = Int16;
  ALint byteblockalign = 0;
  ALint splblockalign = 0;
  sf_count_t num_frames;
  ALenum err, format;
  ALsizei num_bytes;
  SNDFILE* sndfile;
  SF_INFO sfinfo;
  ALuint buffer;
  void* membuf;

  /* Open the audio file and check that it's usable. */
  sndfile = sf_open(filename.c_str(), SFM_READ, &sfinfo);
  if (!sndfile) {
    fprintf(stderr, "Could not open audio in %s: %s\n", filename.c_str(),
            sf_strerror(sndfile));
    return 0;
  }
  if (sfinfo.frames < 1) {
    fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", filename.c_str(),
            sfinfo.frames);
    sf_close(sndfile);
    return 0;
  }

  /* Detect a suitable format to load. Formats like Vorbis and Opus use float
   * natively, so load as float to avoid clipping when possible. Formats
   * larger than 16-bit can also use float to preserve a bit more precision.
   */
  switch ((sfinfo.format & SF_FORMAT_SUBMASK)) {
    case SF_FORMAT_PCM_24:
    case SF_FORMAT_PCM_32:
    case SF_FORMAT_FLOAT:
    case SF_FORMAT_DOUBLE:
    case SF_FORMAT_VORBIS:
    case SF_FORMAT_OPUS:
    case SF_FORMAT_ALAC_20:
    case SF_FORMAT_ALAC_24:
    case SF_FORMAT_ALAC_32:
    case 0x0080 /*SF_FORMAT_MPEG_LAYER_I*/:
    case 0x0081 /*SF_FORMAT_MPEG_LAYER_II*/:
    case 0x0082 /*SF_FORMAT_MPEG_LAYER_III*/:
      if (alIsExtensionPresent("AL_EXT_FLOAT32")) sample_format = Float;
      break;
    case SF_FORMAT_IMA_ADPCM:
      /* ADPCM formats require setting a block alignment as specified in the
       * file, which needs to be read from the wave 'fmt ' chunk manually
       * since libsndfile doesn't provide it in a format-agnostic way.
       */
      if (sfinfo.channels <= 2 &&
          (sfinfo.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV &&
          alIsExtensionPresent("AL_EXT_IMA4") &&
          alIsExtensionPresent("AL_SOFT_block_alignment"))
        sample_format = IMA4;
      break;
    case SF_FORMAT_MS_ADPCM:
      if (sfinfo.channels <= 2 &&
          (sfinfo.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV &&
          alIsExtensionPresent("AL_SOFT_MSADPCM") &&
          alIsExtensionPresent("AL_SOFT_block_alignment"))
        sample_format = MSADPCM;
      break;
  }

  if (sample_format == IMA4 || sample_format == MSADPCM) {
    /* For ADPCM, lookup the wave file's "fmt " chunk, which is a
     * WAVEFORMATEX-based structure for the audio format.
     */
    SF_CHUNK_INFO inf = {"fmt ", 4, 0, NULL};
    SF_CHUNK_ITERATOR* iter = sf_get_chunk_iterator(sndfile, &inf);

    /* If there's an issue getting the chunk or block alignment, load as
     * 16-bit and have libsndfile do the conversion.
     */
    if (!iter || sf_get_chunk_size(iter, &inf) != SF_ERR_NO_ERROR ||
        inf.datalen < 14)
      sample_format = Int16;
    else {
      ALubyte* fmtbuf = static_cast<ALubyte*>(calloc(inf.datalen, 1));
      inf.data = fmtbuf;
      if (sf_get_chunk_data(iter, &inf) != SF_ERR_NO_ERROR)
        sample_format = Int16;
      else {
        /* Read the nBlockAlign field, and convert from bytes- to
         * samples-per-block (verifying it's valid by converting back
         * and comparing to the original value).
         */
        byteblockalign = fmtbuf[12] | (fmtbuf[13] << 8);
        if (sample_format == IMA4) {
          splblockalign = (byteblockalign / sfinfo.channels - 4) / 4 * 8 + 1;
          if (splblockalign < 1 ||
              ((splblockalign - 1) / 2 + 4) * sfinfo.channels != byteblockalign)
            sample_format = Int16;
        } else {
          splblockalign = (byteblockalign / sfinfo.channels - 7) * 2 + 2;
          if (splblockalign < 2 ||
              ((splblockalign - 2) / 2 + 7) * sfinfo.channels != byteblockalign)
            sample_format = Int16;
        }
      }
      free(fmtbuf);
    }
  }

  if (sample_format == Int16) {
    splblockalign = 1;
    byteblockalign = sfinfo.channels * 2;
  } else if (sample_format == Float) {
    splblockalign = 1;
    byteblockalign = sfinfo.channels * 4;
  }

  /* Figure out the OpenAL format from the file and desired sample type. */
  format = AL_NONE;
  if (sfinfo.channels == 1) {
    if (sample_format == Int16)
      format = AL_FORMAT_MONO16;
    else if (sample_format == Float)
      format = AL_FORMAT_MONO_FLOAT32;
    else if (sample_format == IMA4)
      format = AL_FORMAT_MONO_IMA4;
    else if (sample_format == MSADPCM)
      format = AL_FORMAT_MONO_MSADPCM_SOFT;
  } else if (sfinfo.channels == 2) {
    if (sample_format == Int16)
      format = AL_FORMAT_STEREO16;
    else if (sample_format == Float)
      format = AL_FORMAT_STEREO_FLOAT32;
    else if (sample_format == IMA4)
      format = AL_FORMAT_STEREO_IMA4;
    else if (sample_format == MSADPCM)
      format = AL_FORMAT_STEREO_MSADPCM_SOFT;
  } else if (sfinfo.channels == 3) {
    if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) ==
        SF_AMBISONIC_B_FORMAT) {
      if (sample_format == Int16)
        format = AL_FORMAT_BFORMAT2D_16;
      else if (sample_format == Float)
        format = AL_FORMAT_BFORMAT2D_FLOAT32;
    }
  } else if (sfinfo.channels == 4) {
    if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) ==
        SF_AMBISONIC_B_FORMAT) {
      if (sample_format == Int16)
        format = AL_FORMAT_BFORMAT3D_16;
      else if (sample_format == Float)
        format = AL_FORMAT_BFORMAT3D_FLOAT32;
    }
  }
  if (!format) {
    fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
    sf_close(sndfile);
    return 0;
  }

  if (sfinfo.frames / splblockalign > (sf_count_t)(INT_MAX / byteblockalign)) {
    fprintf(stderr, "Too many samples in %s (%" PRId64 ")\n", filename,
            sfinfo.frames);
    sf_close(sndfile);
    return 0;
  }

  /* Decode the whole audio file to a buffer. */
  membuf = malloc((size_t)(sfinfo.frames / splblockalign * byteblockalign));

  if (sample_format == Int16)
    num_frames =
        sf_readf_short(sndfile, static_cast<short*>(membuf), sfinfo.frames);
  else if (sample_format == Float)
    num_frames =
        sf_readf_float(sndfile, static_cast<float*>(membuf), sfinfo.frames);
  else {
    sf_count_t count = sfinfo.frames / splblockalign * byteblockalign;
    num_frames = sf_read_raw(sndfile, membuf, count);
    if (num_frames > 0)
      num_frames = num_frames / byteblockalign * splblockalign;
  }
  if (num_frames < 1) {
    free(membuf);
    sf_close(sndfile);
    fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", filename,
            num_frames);
    return 0;
  }
  num_bytes = (ALsizei)(num_frames / splblockalign * byteblockalign);
#ifdef DEBUG
  printf("Loading: %s (%s, %dhz)\n", filename, FormatName(format),
         sfinfo.samplerate);
#endif
  fflush(stdout);

  /* Buffer the audio data into a new buffer object, then free the data and
   * close the file.
   */
  buffer = 0;
  alGenBuffers(1, &buffer);
  if (splblockalign > 1)
    alBufferi(buffer, AL_UNPACK_BLOCK_ALIGNMENT_SOFT, splblockalign);
  alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

  free(membuf);
  sf_close(sndfile);

  /* Check if an error occurred, and clean up if so. */
  err = alGetError();
  if (err != AL_NO_ERROR) {
    fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
    if (buffer && alIsBuffer(buffer)) alDeleteBuffers(1, &buffer);
    return 0;
  }

  buffers_[name] = buffer;
  default_volumes_[name] = defaultVolume;

  return buffer;
}

void SoundEngine::UnloadSound(const std::string& name) {
  alDeleteBuffers(1, &buffers_.at(name));
  ALenum error = alGetError();
  if (error != AL_NO_ERROR) {
    fprintf(stderr, "Failed to delete buffer: %s\n", alGetString(error));
  }
  buffers_.erase(name);
  default_volumes_.erase(name);
  play_counts_.erase(name);
}

int SoundEngine::GetPlayCount(const std::string& sourceName) {
  assert(buffers_.count(sourceName) > 0 && "Sound not found");
  return play_counts_.count(sourceName) > 0 ? play_counts_.at(sourceName) : 0;
}

void SoundEngine::PlaySound(const std::string& name, bool loop, float volume) {
  auto buffer = buffers_.at(name);
  if (volume < 0.f) {
    volume = default_volumes_.at(name);
  }
  ALuint source;
  alGenSources(1, &source);
  alSourcei(source, AL_BUFFER, buffer);
  alSourcePlay(source);
  if (loop) {
    alSourcei(source, AL_LOOPING, AL_TRUE);
  }
  sources_[name].emplace_back(source);
  SetVolume(source, volume);
  ++play_counts_[name];
}

void SoundEngine::StopSound(const std::string& name) {
  auto sources =
      sources_.at(name);  // Get the source associated with this sound name

  for (const auto& source : sources) {
    // Stop the source
    alSourceStop(source);

    // Delete the source
    alDeleteSources(1, &source);
  }

  // Remove the source from the map
  sources_.erase(name);
}

bool SoundEngine::LoadStream(const std::string& name,
                             const std::string& filename, float defaultVolume) {
  streams_[name] = std::make_unique<StreamPlayer>();
  if (!streams_[name]->Open(filename)) {
    streams_.erase(name);
    return false;
  }
  this->SetStreamState(name, StreamState::READY);
  default_volumes_[name] = defaultVolume;
  return true;
}

void SoundEngine::StreamPlayback(const std::string& streamName) {
  auto& stream = streams_.at(streamName);
  while (GetStreamState(streamName) == StreamState::Playing) {
    if (!stream->Update()) {
      this->SetStreamState(streamName, StreamState::ENDED);
      break;
    }
    std::this_thread::sleep_for(
        std::chrono::nanoseconds{std::chrono::seconds{1}} / refresh_rate_);
  }
}

void SoundEngine::PlayStream(const std::string& name, float volume) {
  if (IsStreamPlaying(name) || IsStreamEnded(name)) {
    // Reset the stream
    ResetStream(name);
  }
  if (volume < 0.f) {
    volume = default_volumes_.at(name);
  }
  auto& stream = streams_.at(name);
  if (!stream->Prepare()) {
    stream->Close();
  }
  auto streamSource = stream->mSource;
  this->SetVolume(streamSource, volume);
  this->SetStreamState(name, StreamState::Playing);
  // Check if there is already a thread for this stream and join it if it's
  // joinable
  auto it = stream_threads_.find(name);
  if (it != stream_threads_.end() && it->second.joinable()) {
    it->second.join();  // Join the existing thread before creating a new one
  }
  // Create a thread for playing the stream and store it in the map
  stream_threads_[name] =
      std::thread([this, name]() { this->StreamPlayback(name); });
}

void SoundEngine::ResetStream(const std::string& name) {
  if (!IsStreamPlaying(name) && !IsStreamEnded(name)) {
    return;
  }
  this->SetStreamState(name, StreamState::READY);
  streams_.at(name)->Reset();
  // Also reset the volume to the default one
  this->SetVolume(streams_.at(name)->mSource, default_volumes_.at(name));
}

bool SoundEngine::IsPlaying(const std::string& sourceName) {
  if (sources_.find(sourceName) != sources_.end()) {
    ALuint source = sources_[sourceName].back();
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
  }
  return false;
}

StreamState SoundEngine::GetStreamState(const std::string& sourceName) {
  std::lock_guard<std::mutex> lock(stream_states_mutex_);
  return stream_states_.at(sourceName);
}

void SoundEngine::SetStreamState(const std::string& sourceName,
                                 StreamState state) {
  std::lock_guard<std::mutex> lock(stream_states_mutex_);
  stream_states_[sourceName] = state;
}

bool SoundEngine::IsStreamPlaying(const std::string& sourceName) {
  return GetStreamState(sourceName) == StreamState::Playing;
}

bool SoundEngine::IsStreamEnded(const std::string& sourceName) {
  return GetStreamState(sourceName) == StreamState::ENDED;
}

void SoundEngine::GraduallyChangeVolume(const std::string& sourceName,
                                        float targetVolume, float duration) {
  targetVolume = std::clamp(targetVolume, 0.0f, 1.0f);
  // Check if the source still exists. It may have been removed if the
  // application lost focus for an extended period.
  if (sources_.find(sourceName) == sources_.end()) {
    return;
  }
  ALuint source = sources_.at(sourceName).back();
  float curVolume = GetVolume(source);
  assert(targetVolume != curVolume && "Target volume is the same as current");
  auto equationParams =
      solveQuadratic(curVolume, glm::vec2(duration, targetVolume));
  gradually_changing_volumes_[sourceName][source] =
      solveQuadratic(curVolume, glm::vec2(duration, targetVolume));
}

void SoundEngine::GraduallyChangeStreamVolume(const std::string& sourceName,
                                              float targetVolume,
                                              float duration) {
  // Only change the volume if the stream is playing
  if (!IsStreamPlaying(sourceName)) {
    return;
  }
  targetVolume = std::clamp(targetVolume, 0.0f, 1.0f);
  float curVolume = GetVolume(streams_.at(sourceName)->mSource);
  assert(targetVolume != curVolume && "Target volume is the same as current");
  auto equationParams =
      solveQuadratic(curVolume, glm::vec2(duration, targetVolume));
  gradually_changing_stream_volumes_[sourceName] =
      solveQuadratic(curVolume, glm::vec2(duration, targetVolume));
}

bool SoundEngine::IsGraduallyChangingVolume(const std::string& sourceName) {
  return gradually_changing_volumes_.find(sourceName) !=
         gradually_changing_volumes_.end();
}

bool SoundEngine::IsGraduallyChangingStreamVolume(
    const std::string& sourceName) {
  return gradually_changing_stream_volumes_.find(sourceName) !=
         gradually_changing_stream_volumes_.end();
}

void SoundEngine::UpdateSourcesVolume(float dt) {
  std::vector<std::string> sourceNamesToDelete;
  for (auto& sourceName : gradually_changing_volumes_) {
    // If the source is not in the sources map, remove it from the gradually
    // changing volumes map.
    if (sources_.find(sourceName.first) == sources_.end()) {
      sourceNamesToDelete.emplace_back(sourceName.first);
      continue;
    }
    std::unordered_set<ALuint> sourcesToDelete;
    for (auto& source : sourceName.second) {
      // If the source is not in the sources map, remove it from the gradually
      // changing volumes map.
      if (std::find(sources_[sourceName.first].begin(),
                    sources_[sourceName.first].end(),
                    source.first) == sources_[sourceName.first].end()) {
        gradually_changing_volumes_[sourceName.first].erase(source.first);
        continue;
      }
      const auto& quadraticParams = source.second;
      float currentVolume = GetVolume(source.first);
      if (currentVolume == 0.f) {
        std::cout << "Volume is 0, removing source" << std::endl;
      }
      auto potentialCurrentTimePoint =
          getXOfQuadratic(currentVolume, quadraticParams).value();
      float currentTimePoint = std::min(potentialCurrentTimePoint.first,
                                        potentialCurrentTimePoint.second);
      float duration = -quadraticParams.y / (2 * quadraticParams.x);
      assert(currentTimePoint >= 0.f &&
             !areFloatsGreater(currentTimePoint, duration) &&
             "Invalid time point for gradually changing volume");
      float newTimePoint = currentTimePoint + dt;
      if (newTimePoint > duration) {
        newTimePoint = duration;
        sourcesToDelete.insert(source.first);
      }
      float newVolume = getYOfQuadratic(newTimePoint, quadraticParams);
      SetVolume(source.first, newVolume);
    }
    for (const auto& source : sourcesToDelete) {
      gradually_changing_volumes_[sourceName.first].erase(source);
    }
    if (gradually_changing_volumes_[sourceName.first].empty()) {
      sourceNamesToDelete.emplace_back(sourceName.first);
    }
  }
  for (const auto& sourceName : sourceNamesToDelete) {
    gradually_changing_volumes_.erase(sourceName);
  }
}

void SoundEngine::UpdateStreamsVolume(float dt) {
  std::vector<std::string> sourceNamesToDelete;
  for (auto& [sourceName, changeEquation] :
       gradually_changing_stream_volumes_) {
    if (!IsStreamPlaying(sourceName)) {
      sourceNamesToDelete.emplace_back(sourceName);
      continue;
    }
    auto& stream = streams_.at(sourceName);
    float currentVolume = GetVolume(stream->mSource);
    auto potentialCurrentTimePoint =
        getXOfQuadratic(currentVolume, changeEquation).value();
    float currentTimePoint = std::min(potentialCurrentTimePoint.first,
                                      potentialCurrentTimePoint.second);
    float duration = -changeEquation.y / (2 * changeEquation.x);
    assert(currentTimePoint >= 0.f &&
           !areFloatsGreater(currentTimePoint, duration) &&
           "Invalid time point for gradually changing stream volume");
    float newTimePoint = currentTimePoint + dt;
    if (newTimePoint > duration) {
      newTimePoint = duration;
      sourceNamesToDelete.emplace_back(sourceName);
    }
    float newVolume = getYOfQuadratic(newTimePoint, changeEquation);
    SetVolume(stream->mSource, newVolume);
  }
  for (const auto& sourceName : sourceNamesToDelete) {
    gradually_changing_stream_volumes_.erase(sourceName);
  }
}

void SoundEngine::SetVolume(const std::string& sourceName, float volume) {
  SetVolume(sources_.at(sourceName).back(), volume);
}

float SoundEngine::GetVolume(const std::string& sourceName) {
  return GetVolume(sources_.at(sourceName).back());
}

float SoundEngine::GetPlaybackPosition(const std::string& sourceName) {
  return GetPlaybackPosition(sources_.at(sourceName).back());
}

float SoundEngine::GetRemainingTime(const std::string& sourceName,
                                    float totalDuration) {
  return GetRemainingTime(sources_.at(sourceName).back(), totalDuration);
}

bool SoundEngine::IsLooping(ALuint source) {
  ALint loop;
  alGetSourcei(source, AL_LOOPING, &loop);
  return loop == AL_TRUE;
}

bool SoundEngine::IsPlaying(ALuint source) {
  ALint state;
  alGetSourcei(source, AL_SOURCE_STATE, &state);
  return state == AL_PLAYING;
}

void SoundEngine::SetVolume(ALuint source, float volume) {
  // Check if the volume is within the valid range
  volume = std::clamp(volume, 0.0f, 1.0f);
  alSourcef(source, AL_GAIN, volume);
}

float SoundEngine::GetVolume(ALuint source) {
  float volume = 0.0f;
  alGetSourcef(source, AL_GAIN, &volume);
  return volume;
}

void SoundEngine::SetBackgroundMusicNames(const std::vector<std::string>& music,
                                          bool isFighting) {
  if (isFighting) {
    background_music_info_.fightingMusic = music;
  } else {
    background_music_info_.relaxingMusic = music;
  }
}

void SoundEngine::StartBackgroundMusic(bool isFighting) {
  BackgroundMusicState state = isFighting ? BackgroundMusicState::Fighting
                                          : BackgroundMusicState::Relaxing;
  if (state == background_music_info_.state) {
    return;
  }
  SetBackgroundMusicState(state);
  std::string currentMusic = background_music_info_.currentMusic;
  if (!currentMusic.empty() &&
      (IsStreamPlaying(currentMusic) || IsStreamEnded(currentMusic))) {
    // Reset the stream
    ResetStream(currentMusic);
  }
  switch (state) {
    case BackgroundMusicState::Fighting:
      background_music_info_.currentMusic =
          background_music_info_.fightingMusic[0];
      break;
    case BackgroundMusicState::Relaxing:
      background_music_info_.currentMusic =
          background_music_info_.relaxingMusic[0];
      break;
    default:
      break;
  }
  background_music_info_.timerInterval = 0.f;
  background_music_info_.lastMusic = "";
  // start streaming the background music
  PlayStream(background_music_info_.currentMusic);
}

void SoundEngine::RefreshBackgroundMusic(float dt) {
  if (background_music_info_.state == BackgroundMusicState::None) {
    return;
  } else {
    assert(!background_music_info_.currentMusic.empty() &&
           "No background music was playing");

    if (!IsStreamEnded(background_music_info_.currentMusic)) {
      return;
    }
    background_music_info_.timerInterval += dt;
    if (background_music_info_.timerInterval >=
        background_music_info_.expectedInterval) {
      background_music_info_.Refresh();
      PlayStream(background_music_info_.currentMusic);
    }
  }
}

void SoundEngine::DoNotPlayNextBackgroundMusic() {
  background_music_info_.StopAtEndOfCurrentMusic();
}

float SoundEngine::GetPlaybackPosition(ALuint source) {
  float currentTime = 0.0f;
  alGetSourcef(source, AL_SEC_OFFSET, &currentTime);
  return currentTime;
}

float SoundEngine::GetRemainingTime(ALuint source, float totalDuration) {
  float currentTime = 0.0f;
  alGetSourcef(source, AL_SEC_OFFSET, &currentTime);
  return totalDuration - currentTime;
}
