#pragma once

// Define DEBUG if in debug mode and not already defined
#if defined(_DEBUG) && !defined(DEBUG)
#define DEBUG
#endif

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <sndfile.h>

#include <al.h>
#include <alc.h>
#include <alext.h>

#include "alhelpers.h"

extern LPALBUFFERCALLBACKSOFT alBufferCallbackSOFT;

struct StreamPlayer {
  /* A lockless ring-buffer (supports single-provider, single-consumer
   * operation).
   */
  std::vector<std::byte> mBufferData;
  std::atomic<size_t> mReadPos{0};
  std::atomic<size_t> mWritePos{0};
  size_t mSamplesPerBlock{1};
  size_t mBytesPerBlock{1};

  enum class SampleType { Int16, Float, IMA4, MSADPCM };
  SampleType mSampleFormat{SampleType::Int16};

  /* The buffer to get the callback, and source to play with. */
  ALuint mBuffer{0}, mSource{0};
  size_t mStartOffset{0};

  /* Handle for the audio file to decode. */
  SNDFILE *mSndfile{nullptr};
  SF_INFO mSfInfo{};
  size_t mDecoderOffset{0};

  /* The format of the callback samples. */
  ALenum mFormat{};

  StreamPlayer();
  ~StreamPlayer();

  void Close();

  bool Open(const std::string &filename);

  /* The actual C-style callback just forwards to the non-static method. Not
   * strictly needed and the compiler will optimize it to a normal function,
   * but it allows the callback implementation to have a nice 'this' pointer
   * with normal member access.
   */
  static ALsizei AL_APIENTRY BufferCallbackC(void *userptr, void *data,
                                             ALsizei size) noexcept;
  ALsizei BufferCallback(void *data, ALsizei size) noexcept;

  bool Prepare();

  bool Update();

  void Reset();
};
