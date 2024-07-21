# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.20)

project ("DynastysDefender-ScrollsCurse")

# Global C++ standard for all sub-projects
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG:FULL")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Set the output directory for all runtime targets (executables)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# set the resources directory
set(SHADERS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/shaders")
set(SHADERS_DEST_DIR "${CMAKE_BINARY_DIR}/shaders")
message(STATUS "============================Source Shaders=================================: ${SHADERS_SOURCE_DIR}")
message(STATUS "===========================Destination Shaders==========================: ${SHADERS_DEST_DIR}")
set(FONTS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/fonts")
set(FONTS_DEST_DIR "${CMAKE_BINARY_DIR}/fonts")
message(STATUS "============================Source Fonts=================================: ${FONTS_SOURCE_DIR}")
message(STATUS "===========================Destination Fonts==========================: ${FONTS_DEST_DIR}")
set(SETTINGS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/settings")
set(SETTINGS_DEST_DIR "${CMAKE_BINARY_DIR}/settings")
message(STATUS "============================Source Settings=================================: ${SETTINGS_SOURCE_DIR}")
message(STATUS "===========================Destination Settings==========================: ${SETTINGS_DEST_DIR}")
set(TEXT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/text")
set(TEXT_DEST_DIR "${CMAKE_BINARY_DIR}/text")
message(STATUS "============================Source Text=================================: ${TEXT_SOURCE_DIR}")
message(STATUS "===========================Destination Text==========================: ${TEXT_DEST_DIR}")
set(TEXTURES_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/textures")
set(TEXTURES_DEST_DIR "${CMAKE_BINARY_DIR}/textures")
message(STATUS "============================Source Textures=================================: ${TEXTURES_SOURCE_DIR}")
message(STATUS "===========================Destination Textures==========================: ${TEXTURES_DEST_DIR}")
set(AUDIO_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/audio")
set(AUDIO_DEST_DIR "${CMAKE_BINARY_DIR}/audio")
message(STATUS "============================Source Audio=================================: ${AUDIO_SOURCE_DIR}")
message(STATUS "===========================Destination Audio==========================: ${AUDIO_DEST_DIR}")

add_subdirectory ("src")
