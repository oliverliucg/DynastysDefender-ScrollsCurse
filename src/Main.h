// PuzzleBobble.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

#include "ConfigManager.h"
#include "GameManager.h"
#include "ResourceManager.h"

// TODO: Reference additional headers your program requires here.
// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode);
