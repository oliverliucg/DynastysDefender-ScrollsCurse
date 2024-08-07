/*
 * Main.cpp : Defines the entry point for the game.
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

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>

#include "ConfigManager.h"
#include "GameManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SoundEngine.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <windows.h>           // Required for Windows API functions
#include <GLFW/glfw3native.h>  // For accessing native window handles

// window dimensions
glm::vec2 kWindowedModeSize = glm::vec2(3840, 2160);
glm::vec2 kFullScreenSize = glm::vec2(3840, 2160);
SizePadding kFullScreenSizePadding = SizePadding();
glm::vec2 kWindowSize = glm::vec2(3840, 2160);

std::atomic<bool> windowShouldClose(false);

// void showTaskbar();
// void hideTaskbar();

void setWindowIcon(GLFWwindow* window);

std::string getSystemLanguage();

std::string getGameTitleBasedOnSystemLanguage();

void initParametersForCurrentWindowSize(int width, int height);
void reconfigureWindowSize(GLFWwindow* window, int width, int height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
// void focus_callback(GLFWwindow* window, int focused);

void recreateWindow(GLFWwindow*& window, int width, int height,
                    GameManager& gameManager);

void handleScreenModeChange(GLFWwindow*& window, GameManager& gameManager,
                            bool& isFromWindowedBorderlessMode);

int main() {
  // Load configurations
  ConfigManager& configManager = ConfigManager::GetInstance();
  configManager.SetConfigPath("settings/config.json");
  if (!configManager.LoadConfig()) {
    std::cerr << "Failed to load configurations." << std::endl;
    return EXIT_FAILURE;
  }
  // Set the default locale based on the system language if it is the first run
  // of the game.
  if (configManager.IsFirstRun()) {
    configManager.SetLanguage(language_enum_map.at(getSystemLanguage()));
  }
  // Initialize GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_RESIZABLE, true);

  // Set the opengl window width to 80% of the screen width and height to 90% of
  // the screen height
  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

  // If it is a full screen mode, then we need to adjust the size to match the
  // aspect ratio of the virtual screen size, which is 16:9
  kFullScreenSizePadding = adjustToAspectRatio(
      mode->width, mode->height, kVirtualScreenSize.x, kVirtualScreenSize.y);
  kFullScreenSize =
      glm::vec2(kFullScreenSizePadding.width, kFullScreenSizePadding.height);

  auto kWindowedModeSizePadding =
      adjustToAspectRatio((mode->width * 7) / 8, (mode->height * 7) / 8,
                          kVirtualScreenSize.x, kVirtualScreenSize.y);
  kWindowedModeSizePadding.padBottom = kWindowedModeSizePadding.padTop =
      kWindowedModeSizePadding.padLeft = kWindowedModeSizePadding.padRight = 0;
  kWindowedModeSize = glm::vec2(kWindowedModeSizePadding.width,
                                kWindowedModeSizePadding.height);
  initParametersForCurrentWindowSize(kVirtualScreenSize.x,
                                     kVirtualScreenSize.y);
  ScreenMode initialScreenMode = configManager.GetScreenMode();

  // By default, the screen mode is set to full screen mode
  SizePadding SCREEN_SIZE_PADDING = kFullScreenSizePadding;
  int SCREEN_WIDTH = SCREEN_SIZE_PADDING.GetPaddedWidth(),
      SCREEN_HEIGHT = SCREEN_SIZE_PADDING.GetPaddedHeight();
  GLFWwindow* window;
  std::string title = getGameTitleBasedOnSystemLanguage();
  if (initialScreenMode == ScreenMode::FULLSCREEN) {
    // Full screen mode
    window = glfwCreateWindow(mode->width, mode->height, title.c_str(),
                              primaryMonitor, NULL);
  } else if (initialScreenMode == ScreenMode::WINDOWED_BORDERLESS) {
    // Windowed borderless mode
    window =
        glfwCreateWindow(mode->width, mode->height, title.c_str(), NULL, NULL);
  } else if (initialScreenMode == ScreenMode::WINDOWED) {
    SCREEN_SIZE_PADDING = kWindowedModeSizePadding;
    // No padding for windowed mode
    SCREEN_WIDTH = kWindowedModeSize.x;
    SCREEN_HEIGHT = kWindowedModeSize.y;
    // Windowed mode
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, title.c_str(), NULL,
                              NULL);
  }
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Set the app icon
  setWindowIcon(window);

  // Set the position of window to the center of the screen
  int windowPosX = (mode->width - SCREEN_WIDTH) / 2;
  int windowPosY = (mode->height - SCREEN_HEIGHT) / 2;
  glfwSetWindowPos(window, windowPosX, windowPosY);
  glfwMakeContextCurrent(window);

  // Use GLAD to load OpenGL functions
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return EXIT_FAILURE;
  }

  // OpenGL configuration
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // initialize game
  int GAME_AREA_WIDTH = kVirtualScreenSize.x;
  int GAME_AREA_HEIGHT = kVirtualScreenSize.y;
  assert(GAME_AREA_WIDTH > 0 && GAME_AREA_HEIGHT > 0 && "Invalid screen size!");
  // Clear the resources
  ResourceManager::GetInstance().Clear();
  Renderer::SetExpectedWindowSizePadding(kVirtualScreenSizePadding);
  Renderer::SetActualWindowSizePadding(SCREEN_SIZE_PADDING);
  GameManager gameManager(GAME_AREA_WIDTH, GAME_AREA_HEIGHT);
  gameManager.PreLoad();

  if (gameManager.screenMode == ScreenMode::WINDOWED_BORDERLESS) {
    gameManager.targetScreenMode = ScreenMode::WINDOWED_BORDERLESS;
  }

  glfwSetWindowUserPointer(window, &gameManager);

  int framebufferWidth, framebufferHeight;
  glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
  reconfigureWindowSize(window, framebufferWidth, framebufferHeight);
  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // check if it is set to windowed borderless mode previously before setting
  // to windowed mode.
  bool isFromWindowedBorderlessMode =
      initialScreenMode == ScreenMode::WINDOWED_BORDERLESS;

  // fixed time step
  const float kTimeStep = 1.f / 240.f;

  float accumulator = 0.f;
  float deltaTime = 0.0f;
  float lastFrame = static_cast<float>(glfwGetTime());
  float startFrame = static_cast<float>(glfwGetTime());
  // Start a loop that runs until the user closes the window
  while (!glfwWindowShouldClose(window)) {
    // Calculate delta time
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    accumulator += deltaTime;
    if (gameManager.state == GameState::PRELOAD &&
        gameManager.targetState == GameState::UNDEFINED &&
        currentFrame - startFrame >= 2.85f) {
      gameManager.Init();
      gameManager.LoadSounds();
      gameManager.LoadStreams();
      assert(gameManager.targetState == GameState::SPLASH_SCREEN &&
             "Failed to initialize the game.");
      int framebufferWidth, framebufferHeight;
      glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
      reconfigureWindowSize(window, framebufferWidth, framebufferHeight);
      accumulator = 0.f;
      currentFrame = static_cast<float>(glfwGetTime());
      lastFrame = currentFrame;
      continue;
    }

    // Check if any events have been activiated (key pressed, mouse moved etc.)
    // and call corresponding response functions
    glfwPollEvents();

    if (glfwWindowShouldClose(window)) {
      windowShouldClose.store(true);  // Synchronize close signal with GLFW
    }

    // Process the input at fixed time step and update the game state.
    while (accumulator >= kTimeStep) {
      // Process input
      gameManager.ProcessInput(kTimeStep);

      // Update screen mode if needed
      if (gameManager.targetScreenMode != ScreenMode::UNDEFINED) {
        handleScreenModeChange(window, gameManager,
                               isFromWindowedBorderlessMode);
      }

      // Update game state
      gameManager.Update(kTimeStep);
      accumulator -= kTimeStep;
    }

    // Set the clear color
    glClearColor(0.039216f, 0.043137f, 0.070588f, 1.0f);
    // Clear the colorbuffer
    glClear(GL_COLOR_BUFFER_BIT);
    // Hide the default cursor if needed.
    if (gameManager.hideDefaultMouseCursor) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Render
    gameManager.Render();

    // Swap the screen buffers
    glfwSwapBuffers(window);
  }

  // Delete all resources.
  ResourceManager::GetInstance().Clear();

  // Terminate GLFW, clearing any resources allocated by GLFW.
  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}

// void showTaskbar() {
//   HWND taskbar = FindWindow("Shell_TrayWnd", NULL);
//   if (taskbar) {
//     SetWindowPos(taskbar, 0, 0, 0, 0, 0, SWP_SHOWWINDOW);
//   }
// }

// void hideTaskbar() {
//   HWND taskbar = FindWindow("Shell_TrayWnd", NULL);
//   if (taskbar) {
//     SetWindowPos(taskbar, 0, 0, 0, 0, 0, SWP_HIDEWINDOW);
//   }
// }

void setWindowIcon(GLFWwindow* window) {
  // Load the icon
  GLFWimage icon;
  icon.pixels =
      stbi_load("icon/game_icon.png", &icon.width, &icon.height, 0, 4);
  if (icon.pixels) {
    glfwSetWindowIcon(window, 1, &icon);
    stbi_image_free(icon.pixels);  // Free the image memory
  } else {
    std::cerr << "Failed to load icon: " << stbi_failure_reason() << std::endl;
  }
}

std::string getSystemLanguage() {
  WCHAR locale[LOCALE_NAME_MAX_LENGTH];
  std::string name = "en-US";
  std::string abbr = "en";
  if (GetUserDefaultLocaleName(locale, LOCALE_NAME_MAX_LENGTH) > 0) {
    name = std::string(locale, locale + wcslen(locale));
  }
  if (name == "pt-BR" || name == "pt-PT") {
    abbr = name;
  } else {
    size_t dashPos = name.find('-');
    if (dashPos != std::string::npos) {
      // language code.
      abbr = name.substr(0, dashPos);
      if (abbr == "zh") {
        // region code.
        std::string region = name.substr(dashPos + 1);
        if (region == "CN") {
          abbr = "zh-Hans";
        } else {
          abbr = "zh-Hant";
        }
      }
    } else {
      abbr = name;
    }
  }
  if (abbr != "de" && abbr != "en" && abbr != "es" && abbr != "fr" &&
      abbr != "it" && abbr != "ja" && abbr != "ko" && abbr != "pt-BR" &&
      abbr != "pt-PT" && abbr != "ru" && abbr != "zh-Hans" &&
      abbr != "zh-Hant") {
    abbr = "en";
  }
  return abbr;
}

std::string getGameTitleBasedOnSystemLanguage() {
  std::string locale = getSystemLanguage();
  // Load the text for the game title.
  ResourceManager& resourceManager = ResourceManager::GetInstance();
  resourceManager.LoadText(
      ConfigManager::GetInstance().GetSystemTextFilePath(locale).c_str());
  std::string gameTitle = resourceManager.GetTextUTF8("title");
  return gameTitle;
}

void initParametersForCurrentWindowSize(int width, int height) {
  kWindowSize = glm::vec2(width, height);
  screenScale = width / kFullScreenSize.x;
  kBaseUnit = kWindowSize.y / 42.f;
  kBubbleRadius = kBaseUnit;
  kVelocityUnit = 2 * kBaseUnit;
  kBubbleSize = glm::vec2(kBubbleRadius * 2, kBubbleRadius * 2);
  kFontScale = 0.2f;
  kFontSize = kWindowSize.y * kFontScale;
}

void reconfigureWindowSize(GLFWwindow* window, int width, int height) {
  // Adjust the window size to match the aspect ratio of the virtual screen
  // size, which is 16:9
  SizePadding adjustedSizes = adjustToAspectRatio(
      width, height, kVirtualScreenSize.x, kVirtualScreenSize.y);

  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  int vpX = adjustedSizes.padLeft;
  int vpY = adjustedSizes.padTop;
  int vpWidth = width - (adjustedSizes.padLeft + adjustedSizes.padRight);
  int vpHeight = height - (adjustedSizes.padTop + adjustedSizes.padBottom);
  glViewport(vpX, vpY, vpWidth, vpHeight);
  if (vpWidth == 0 || vpHeight == 0) {
    return;
  }
  // Retrieve the pointer to gamemanager
  GameManager* gameManager =
      static_cast<GameManager*>(glfwGetWindowUserPointer(window));

  // Resize PostProcessing FBO
  if (gameManager->GetPostProcessor() != NULL) {
    /*gameManager->GetPostProcessor()->Resize(adjustedSizes);*/
    ViewPortInfo newDstViewPortInfo = {vpX, vpY, vpWidth, vpHeight};
    gameManager->GetPostProcessor()->SetDstViewPort(newDstViewPortInfo);
  }

  // memorize the actual window size padding
  Renderer::SetActualWindowSizePadding(adjustedSizes);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  reconfigureWindowSize(window, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode) {
  // Retrieve the pointer to gamemanager
  GameManager* gameManager =
      static_cast<GameManager*>(glfwGetWindowUserPointer(window));
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS)
      gameManager->keys[key] = true;
    else if (action == GLFW_RELEASE) {
      gameManager->keys[key] = false;
      gameManager->keysLocked[key] = false;
    }
  }
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
  // Retrieve the pointer to your game manager
  GameManager* gameManager =
      static_cast<GameManager*>(glfwGetWindowUserPointer(window));

  // Handle the scroll event
  gameManager->scrollYOffset = static_cast<float>(yOffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods) {
  GameManager* gameManager =
      static_cast<GameManager*>(glfwGetWindowUserPointer(window));

  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      if (gameManager->leftMousePressed == false) {
        gameManager->leftMousePressed = true;
        gameManager->mouseLastX = gameManager->mouseX;
        gameManager->mouseLastY = gameManager->mouseY;
      }
    } else if (action == GLFW_RELEASE) {
      // When left mouse button is released, stop dragging
      gameManager->leftMousePressed = false;
      gameManager->isReadyToDrag = true;
      gameManager->isDragging = false;
    }
  }

  // Set the window to be close when the game state is EXIT
  if (gameManager->state == GameState::EXIT) {
    glfwSetWindowShouldClose(window, true);
  }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
  GameManager* gameManager =
      static_cast<GameManager*>(glfwGetWindowUserPointer(window));

  // Get the mouse position in the virtual screen
  SizePadding actualWindowSizePadding = Renderer::GetActualWindowSizePadding();
  SizePadding expectedWindowSizePadding =
      Renderer::GetExpectedWindowSizePadding();

  float virtualMouseX = (xpos - actualWindowSizePadding.padLeft) *
                        expectedWindowSizePadding.width /
                        actualWindowSizePadding.width;
  float virtualMouseY = (ypos - actualWindowSizePadding.padTop) *
                        expectedWindowSizePadding.height /
                        actualWindowSizePadding.height;

  gameManager->mouseX = virtualMouseX;
  gameManager->mouseY = virtualMouseY;
}

// void focus_callback(GLFWwindow* window, int focused) {
//   // Retrieve the pointer to gamemanager
//   GameManager* gameManager =
//       static_cast<GameManager*>(glfwGetWindowUserPointer(window));
//   if (!focused && gameManager->focused) {
//     // Show the taskbar again
//     showTaskbar();
//     gameManager->focused = false;
//   } else if (focused && !gameManager->focused) {
//     // Hide the taskbar
//     hideTaskbar();
//     gameManager->focused = true;
//   }
// }

void recreateWindow(GLFWwindow*& window, int width, int height,
                    GameManager& gameManager) {
  gameManager.SetToTargetScreenMode();
  auto gameStateSnapshot = gameManager.PrepareToReload();
  // Destroy the current window
  glfwDestroyWindow(window);  // Destroy the old window
  // Recreate the window
  window = glfwCreateWindow(width, height, "DynastysDefender-ScrollsCurse",
                            NULL, NULL);

  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }
  // Set the position of window to the center of the screen
  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
  int windowPosX = (mode->width - width) / 2;
  int windowPosY = (mode->height - height) / 2;
  glfwSetWindowPos(window, windowPosX, windowPosY);
  // Make the new window's context current
  glfwMakeContextCurrent(window);

  // OpenGL configuration
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  gameManager.Reload(gameStateSnapshot);

  // if (gameManager.screenMode == ScreenMode::WINDOWED_BORDERLESS) {
  //   hideTaskbar();
  // }

  glfwSetWindowUserPointer(window, &gameManager);

  int framebufferWidth, framebufferHeight;
  glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
  reconfigureWindowSize(window, framebufferWidth, framebufferHeight);
  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void handleScreenModeChange(GLFWwindow*& window, GameManager& gameManager,
                            bool& isFromWindowedBorderlessMode) {
  int SCREEN_WIDTH, SCREEN_HEIGHT;
  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
  if (gameManager.targetScreenMode == ScreenMode::WINDOWED) {
    SCREEN_WIDTH = kWindowedModeSize.x;
    SCREEN_HEIGHT = kWindowedModeSize.y;
    if (isFromWindowedBorderlessMode) {
      isFromWindowedBorderlessMode = false;
      // Add the styles back to have title bar, resize, and system menu.
      HWND hwnd = glfwGetWin32Window(window);
      LONG style = GetWindowLong(hwnd, GWL_STYLE);
      style |= (WS_CAPTION | WS_THICKFRAME | WS_SYSMENU);
      SetWindowLong(hwnd, GWL_STYLE, style);
      if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
      }
    }
    int windowPosX = (mode->width - SCREEN_WIDTH) / 2;
    int windowPosY = (mode->height - SCREEN_HEIGHT) / 2;
    glfwSetWindowMonitor(window, NULL, windowPosX, windowPosY, SCREEN_WIDTH,
                         SCREEN_HEIGHT, GLFW_DONT_CARE);
    gameManager.SetToTargetScreenMode();
  } else if (gameManager.targetScreenMode == ScreenMode::FULLSCREEN) {
    SCREEN_WIDTH = kFullScreenSize.x + kFullScreenSizePadding.padLeft +
                   kFullScreenSizePadding.padRight;
    SCREEN_HEIGHT = kFullScreenSize.y + kFullScreenSizePadding.padTop +
                    kFullScreenSizePadding.padBottom;
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    glfwSetWindowMonitor(window, primaryMonitor, 0, 0, SCREEN_WIDTH,
                         SCREEN_HEIGHT, mode->refreshRate);
    gameManager.SetToTargetScreenMode();
  } else if (gameManager.targetScreenMode == ScreenMode::WINDOWED_BORDERLESS) {
    if (gameManager.screenMode == ScreenMode::FULLSCREEN) {
      glfwSetWindowMonitor(window, NULL, 0, 0, mode->width, mode->height,
                           GLFW_DONT_CARE);
    }
    HWND hwnd = glfwGetWin32Window(window);
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU);
    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, mode->width, mode->height,
                 SWP_FRAMECHANGED);
    gameManager.SetToTargetScreenMode();
    isFromWindowedBorderlessMode = true;
  }
}
