/*
 * StreamPlayer.h
 *
 * Originally part of the OpenAL Callback-based Stream Example by Chris
 * Robinson. Copyright (c) 2020 by Chris Robinson <chris.kcat@gmail.com>
 * https://github.com/kcat/openal-soft (if applicable)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Modifications:
 * - Split into .hpp and .cpp files for modular integration into the sound
 * engine of the game.
 * - Added Reset functionality to replay streams without needing to reload the
 * music data.
 * - Removed main function to adapt the code for use as a library within the
 * game.
 */
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
