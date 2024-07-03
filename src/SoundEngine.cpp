#include "SoundEngine.h"

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
  std::vector<std::string> sourcesToDelete;
  for (auto& source :
       sources_) {  // Assuming activeSources is a list of source IDs
    ALint state;
    auto sourceID = source.second;
    alGetSourcei(sourceID, AL_SOURCE_STATE, &state);

    // Check if the source is not playing or if force cleanup is enabled
    if (force || (state != AL_PLAYING && !IsLooping(sourceID))) {
      alDeleteSources(1, &sourceID);
      sourcesToDelete.emplace_back(source.first);
    }
  }
  // Remove the deleted sources from the active list
  for (auto sourceName : sourcesToDelete) {
    sources_.erase(sourceName);
  }
}

void SoundEngine::Clear() {
  CleanUpSources(/*force=*/true);
  for (auto& buffer : buffers_) {
    alDeleteBuffers(1, &buffer.second);
  }
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(context_);
  alcCloseDevice(device_);
}

ALuint SoundEngine::LoadSound(const std::string& name,
                              const std::string& filename) {
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

  printf("Loading: %s (%s, %dhz)\n", filename, FormatName(format),
         sfinfo.samplerate);
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

  return buffer;
}

int SoundEngine::GetPlayCount(const std::string& sourceName) {
  assert(buffers_.count(sourceName) > 0 && "Sound not found");
  return play_counts_.count(sourceName) > 0 ? play_counts_.at(sourceName) : 0;
}

void SoundEngine::PlaySound(const std::string& name, bool loop, float volume) {
  auto buffer = buffers_.at(name);
  ALuint source;
  alGenSources(1, &source);
  alSourcei(source, AL_BUFFER, buffer);
  alSourcePlay(source);
  if (loop) {
    alSourcei(source, AL_LOOPING, AL_TRUE);
  }
  sources_[name] = source;
  SetVolume(source, volume);
  ++play_counts_[name];
  //// Wait for the sound to finish
  // ALint source_state;
  // do {
  //   alGetSourcei(source, AL_SOURCE_STATE, &source_state);
  // } while (source_state == AL_PLAYING);
  // alDeleteSources(1, &source);
}

void SoundEngine::StopSound(const std::string& name) {
  ALuint source =
      sources_.at(name);  // Get the source associated with this sound name

  // Stop the source
  alSourceStop(source);

  // Delete the source
  alDeleteSources(1, &source);

  // Remove the source from the map
  sources_.erase(name);
}

bool SoundEngine::IsPlaying(const std::string& sourceName) {
  if (sources_.find(sourceName) != sources_.end()) {
    ALuint source = sources_[sourceName];
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
  }
  return false;
}

void SoundEngine::SetVolume(const std::string& sourceName, float volume) {
  SetVolume(sources_.at(sourceName), volume);
}

float SoundEngine::GetVolume(const std::string& sourceName) {
  return GetVolume(sources_.at(sourceName));
}

float SoundEngine::GetPlaybackPosition(const std::string& sourceName) {
  return GetPlaybackPosition(sources_.at(sourceName));
}

float SoundEngine::GetRemainingTime(const std::string& sourceName,
                                    float totalDuration) {
  return GetRemainingTime(sources_.at(sourceName), totalDuration);
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
  if (volume < 0.0f) {
    volume = 0.0f;
  } else if (volume > 1.0f) {
    volume = 1.0f;
  }
  alSourcef(source, AL_GAIN, volume);
}

float SoundEngine::GetVolume(ALuint source) {
  float volume = 0.0f;
  alGetSourcef(source, AL_GAIN, &volume);
  return volume;
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