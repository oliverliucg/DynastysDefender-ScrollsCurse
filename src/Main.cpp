﻿// Main.cpp : Defines the entry point for the application.

#include "Main.h"
#include "Renderer.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>

// window dimensions
glm::vec2 kVirtualScreenSize = glm::vec2(3840, 2160);
glm::vec2 kWindowedModeSize = glm::vec2(3840, 2160);
glm::vec2 kFullScreenSize = glm::vec2(3840, 2160);
SizePadding kFullScreenSizePadding = SizePadding();
glm::vec2 kWindowSize = glm::vec2(3840, 2160);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// Retrieve the pointer to gamemanager
	GameManager* gameManager = static_cast<GameManager*>(glfwGetWindowUserPointer(window));
	if (key >= 0 && key < 1024)
	{
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
	GameManager* gameManager = static_cast<GameManager*>(glfwGetWindowUserPointer(window));

	// Handle the scroll event
	gameManager->scrollYOffset = static_cast<float>(yOffset);
}

void switchToFullScreen(GLFWwindow* window) {
	// Get the primary monitor
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	// Get the video mode for this monitor
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

	// Switch to full-screen at the monitor's native resolution
	glfwSetWindowMonitor(window, NULL, 0, 0, mode->width, mode->height, mode->refreshRate);
}

void switchToWindowedMode(GLFWwindow* window, int width, int height) {
	// Get the primary monitor
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	// Get the video mode for this monitor
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
	// Switch back to windowed mode
	int windowPosX = (mode->width - width) / 2;
	int windowPosY = (mode->height - height) / 2;
	glfwSetWindowMonitor(window, NULL, windowPosX, windowPosY, width, height, GLFW_DONT_CARE);
}

void SetWindowSize(int width, int height) {
	kWindowSize = glm::vec2(width, height);
	screenScale = width / kFullScreenSize.x;
	/*kBubbleRadius = kWindowSize.y / 42.f;*/
	kVelocityUnit = 2 * kBubbleRadius;
	/*kBubbleSize = glm::vec2(kBubbleRadius * 2, kBubbleRadius * 2);*/
	kFontScale = 0.2f;
	kFontSize = kWindowSize.y * kFontScale;

	Renderer::SetScaleRendering(1.f);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	GameManager* gameManager = static_cast<GameManager*>(glfwGetWindowUserPointer(window));

	if (button == GLFW_MOUSE_BUTTON_LEFT) {	
		if (action == GLFW_PRESS) {
			if (gameManager->leftMousePressed == false) {
				gameManager->leftMousePressed = true;
				gameManager->mouseLastX = gameManager->mouseX;
				gameManager->mouseLastY = gameManager->mouseY;
			}
		}
		else if (action == GLFW_RELEASE) {
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

	//// Swtich the screen mode.
	//if (gameManager->targetScreenMode != ScreenMode::UNDEFINED && gameManager->targetScreenMode != gameManager->screenMode) {
	//	int SCREEN_WIDTH = -1;
	//	int SCREEN_HEIGHT = -1;
	//	if (gameManager->targetScreenMode == ScreenMode::WINDOWED) {
	//		SCREEN_WIDTH = kWindowedModeSize.x;
	//		SCREEN_HEIGHT = kWindowedModeSize.y;
	//	}
	//	else if (gameManager->targetScreenMode == ScreenMode::FULLSCREEN) {
	//		SCREEN_WIDTH = kFullScreenSize.x;
	//		SCREEN_HEIGHT = kFullScreenSize.y;
	//	}
	//	gameManager->screenMode = gameManager->targetScreenMode;
	//	gameManager->targetScreenMode = ScreenMode::UNDEFINED;

	//	/*glfwSetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);*/
	//	if (gameManager->screenMode == ScreenMode::FULLSCREEN) {
	//		switchToFullScreen(window);
	//	}
	//	else {
	//		switchToWindowedMode(window, SCREEN_WIDTH, SCREEN_HEIGHT);
	//	}
	//	//glfwSetWindowShouldClose(window, false);

	//	// Reconfigure the attibutes in the game that are affected by the change of screen size.
	//	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	//	SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	//	gameManager->width = SCREEN_WIDTH;
	//	gameManager->height = SCREEN_HEIGHT;
	//}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	GameManager* gameManager = static_cast<GameManager*>(glfwGetWindowUserPointer(window));
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// Get the mouse position in the virtual screen
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	SizePadding actualWindowSizePadding = ScissorBoxHandler::GetInstance().GetActualWindowSizePadding();
	SizePadding expectedWindowSizePadding = ScissorBoxHandler::GetInstance().GetExpectedWindowSizePadding();
	float virtualMouseX = (mouseX - actualWindowSizePadding.padLeft) * expectedWindowSizePadding.width / actualWindowSizePadding.width + expectedWindowSizePadding.padLeft;
	float virtualMouseY = (mouseY - actualWindowSizePadding.padTop) * expectedWindowSizePadding.height / actualWindowSizePadding.height + expectedWindowSizePadding.padTop;

	gameManager->mouseX = static_cast<float>(virtualMouseX);
	gameManager->mouseY = static_cast<float>(virtualMouseY);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Adjust the window size to match the aspect ratio of the virtual screen size, which is 16:9
	SizePadding adjustedSizes = adjustToAspectRatio(width, height, kVirtualScreenSize.x, kVirtualScreenSize.y);

	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	int vpX = adjustedSizes.padLeft;
	int vpY = adjustedSizes.padTop;
	int vpWidth = width - (adjustedSizes.padLeft + adjustedSizes.padRight);
	int vpHeight = height - (adjustedSizes.padTop + adjustedSizes.padBottom);
	glViewport(vpX, vpY, vpWidth, vpHeight);

	// Retrieve the pointer to gamemanager
	GameManager* gameManager = static_cast<GameManager*>(glfwGetWindowUserPointer(window));
	// Resize PostProcessing FBO
	gameManager->GetPostProcessor()->Resize(width, height);
	// Resize the scissor box handler
	ScissorBoxHandler::GetInstance().SetActualWindowSizePadding(adjustedSizes);
}

int main()
{	
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, false);


	// Set the opengl window width to 80% of the screen width and height to 90% of the screen height
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
	
	// If it is a full screen mode, then we need to adjust the size to match the aspect ratio of the virtual screen size, which is 16:9
	kFullScreenSizePadding = adjustToAspectRatio(mode->width, mode->height, kVirtualScreenSize.x, kVirtualScreenSize.y);
	kFullScreenSize = glm::vec2(kFullScreenSizePadding.width, kFullScreenSizePadding.height);

	auto kWindowedModeSizePadding = adjustToAspectRatio((mode->width * 7) / 8, (mode->height * 7) / 8, kVirtualScreenSize.x, kVirtualScreenSize.y);
	kWindowedModeSize = glm::vec2(kWindowedModeSizePadding.width, kWindowedModeSizePadding.height);
	/*kOptionWindowSize = glm::vec2(OPTION_WINDOW_WIDTH, OPTION_WINDOW_HEIGHT);*/
	SetWindowSize(kFullScreenSize.x, kFullScreenSize.y);
	std::cout << "Resolution: " << mode->width << "x" << mode->height << std::endl;
	std::cout << "SCREEN_WIDTH: " << std::endl;
	std::cout << "WINDOWED_MODE_SCREEN_WIDTH: " << kWindowedModeSize.x << " WINDOWED_MODE_SCREEN_HEIGHT: " << kWindowedModeSize.y << std::endl;
	std::cout << "FULL_SCREEN_WINDOW_WIDTH: " << kFullScreenSize.x << " FULL_SCREEN_WINDOW_HEIGHT: " << kFullScreenSize.y << std::endl;
	/*std::cout << "OPTION_WINDOW_WIDTH: " << OPTION_WINDOW_WIDTH << " OPTION_WINDOW_HEIGHT: " << OPTION_WINDOW_HEIGHT << std::endl;*/
	
	SizePadding SCREEN_SIZE_PADDING = kFullScreenSizePadding;
	int SCREEN_WIDTH = SCREEN_SIZE_PADDING.GetPaddedWidth(), SCREEN_HEIGHT = SCREEN_SIZE_PADDING.GetPaddedHeight();
	// Create the window
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Puzzle Bobble", NULL, NULL);
	/*glfwSetWindowMonitor(window, primaryMonitor, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mode->refreshRate);*/
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;	
	}
	glfwMakeContextCurrent(window);

	// Use GLAD to load OpenGL functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;	
		return EXIT_FAILURE;
	}

	// Set the position of window to the center of the screen
	int windowPosX = (mode->width - SCREEN_WIDTH) / 2;
	int windowPosY = (mode->height - SCREEN_HEIGHT) / 2;
	glfwSetWindowPos(window, windowPosX, windowPosY);

	// OpenGL configuration
	// --------------------
	glViewport(windowPosX, windowPosY, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// initialize game
	assert(SCREEN_WIDTH > 0 && SCREEN_HEIGHT > 0 && "Invalid screen size!");
	GameManager gameManager(SCREEN_WIDTH, SCREEN_HEIGHT);
	ScissorBoxHandler::GetInstance().SetExpectedWindowSizePadding(SCREEN_SIZE_PADDING);
	ScissorBoxHandler::GetInstance().SetActualWindowSizePadding(SCREEN_SIZE_PADDING);
	glfwSetWindowUserPointer(window, &gameManager);

	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// deltaTime variables
	// -------------------
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// Option window to choose between windowed mode and full screen mode
	/*gameManager.state = GameState::OPTION;*/
	gameManager.Init();
	//// Set the background color to (0.988f, 0.928f, 0.828f, 1.0f) which is a pale yellow
	//glClearColor(0.988f, 0.928f, 0.828f, 1.0f);
	// Start a loop that runs until the user chooses which mode to play
	//while(!glfwWindowShouldClose(window)) {

	//	// Clear the color buffer with the specified clear color
	//	glClear(GL_COLOR_BUFFER_BIT);

	//	// Calculate delta time
	//	float currentFrame = static_cast<float>(glfwGetTime());
	//	deltaTime = currentFrame - lastFrame;
	//	lastFrame = currentFrame;

	//	// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
	//	glfwPollEvents();

	//	// Process input
	//	gameManager.ProcessInput(deltaTime);

	//	// Draw the background
	//	gameManager.Render();

	//	// Swap the screen buffers
	//	glfwSwapBuffers(window);
	//}

	// Clear the resources
	ResourceManager::GetInstance().Clear();

	//if (gameManager.screenMode == ScreenMode::WINDOWED) {
	//	SCREEN_WIDTH = WINDOWED_MODE_SCREEN_WIDTH;
	//	SCREEN_HEIGHT = WINDOWED_MODE_SCREEN_HEIGHT;
	//}
	//else if (gameManager.screenMode == ScreenMode::FULLSCREEN) {
	//	SCREEN_WIDTH = FULL_SCREEN_WINDOW_WIDTH;
	//	SCREEN_HEIGHT = FULL_SCREEN_WINDOW_HEIGHT;
	//}
	// Set the position of window to the center of the screen
//	int windowPosX = (mode->width - SCREEN_WIDTH) / 2;
//	int windowPosY = (mode->height - SCREEN_HEIGHT) / 2;
///*	glfwSetWindowPos(window, windowPosX, windowPosY);*/
//	// OpenGL configuration
//	// --------------------
//	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
//	/*glfwSetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);*/
//	if (gameManager.screenMode == ScreenMode::FULLSCREEN) {
//		glfwSetWindowMonitor(window, NULL, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mode->refreshRate);
//	}
//	else {
//		glfwSetWindowMonitor(window, NULL, windowPosX, windowPosY, SCREEN_WIDTH, SCREEN_HEIGHT, GLFW_DONT_CARE);
//	}
//	std::cout << "refresh rate: " << mode->refreshRate << std::endl;
//	std::cout << "screen size: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << std::endl;
//	glfwSetWindowShouldClose(window, false);
//
//	// Reconfigure the attibutes in the game that are affected by the change of screen size.
//	SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
//	gameManager.width = SCREEN_WIDTH;
//	gameManager.height = SCREEN_HEIGHT;


	// Main game loop
	gameManager.Init();
	// Start a loop that runs until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Calculate delta time
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Default pale blue from LearnOpenGL
		glClear(GL_COLOR_BUFFER_BIT);

		// Process input
		gameManager.ProcessInput(deltaTime);

		if (gameManager.targetScreenMode == ScreenMode::WINDOWED) {
			SCREEN_WIDTH = kWindowedModeSize.x;
			SCREEN_HEIGHT = kWindowedModeSize.y;
			int windowPosX = (mode->width - SCREEN_WIDTH) / 2;
			int windowPosY = (mode->height - SCREEN_HEIGHT) / 2;
			glfwSetWindowMonitor(window, NULL, windowPosX, windowPosY, SCREEN_WIDTH, SCREEN_HEIGHT, GLFW_DONT_CARE);
			gameManager.screenMode = gameManager.targetScreenMode;
			gameManager.targetScreenMode = ScreenMode::UNDEFINED;
		}
		else if (gameManager.targetScreenMode == ScreenMode::FULLSCREEN) {
			SCREEN_WIDTH = kFullScreenSize.x;
			SCREEN_HEIGHT = kFullScreenSize.y;
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			glfwSetWindowMonitor(window, NULL, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mode->refreshRate);
			gameManager.screenMode = gameManager.targetScreenMode;
			gameManager.targetScreenMode = ScreenMode::UNDEFINED;
		}

		// Update game state
		gameManager.Update(deltaTime);

		// Draw the background
		gameManager.Render();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Delete all resources.
	ResourceManager::GetInstance().Clear();

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	std::cout << "SCREEN_WIDTH: " << SCREEN_WIDTH << " SCREEN_HEIGHT: " << SCREEN_HEIGHT << std::endl;
	return EXIT_SUCCESS;
}
