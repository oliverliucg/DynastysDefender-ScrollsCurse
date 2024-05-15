// Main.cpp : Defines the entry point for the application.

#include "Main.h"
#include "Renderer.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>

// window dimensions
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
	kBubbleRadius = kWindowSize.y / 42.f;
	kVelocityUnit = 2 * kBubbleRadius;
	kBubbleSize = glm::vec2(kBubbleRadius * 2, kBubbleRadius * 2);
	kFontScale = 0.2f;
	kFontSize = kWindowSize.y * kFontScale;
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
	//double mouseX, mouseY;
	//glfwGetCursorPos(window, &mouseX, &mouseY);

	// Get the mouse position in the virtual screen
	SizePadding actualWindowSizePadding = Renderer::GetActualWindowSizePadding();
	SizePadding expectedWindowSizePadding = Renderer::GetExpectedWindowSizePadding();
	
	float virtualMouseX = (xpos - actualWindowSizePadding.padLeft) * expectedWindowSizePadding.width / actualWindowSizePadding.width;
	float virtualMouseY = (ypos - actualWindowSizePadding.padTop) * expectedWindowSizePadding.height / actualWindowSizePadding.height;
	
	//float expectedWidth = expectedWindowSizePadding.GetPaddedWidth();
	//float expectedHeight = expectedWindowSizePadding.GetPaddedHeight();
	//float actualWidth = actualWindowSizePadding.GetPaddedWidth();
	//float actualHeight = actualWindowSizePadding.GetPaddedHeight();

	//float virtualMouseX = xpos/actualWidth * expectedWidth;
	//float virtualMouseY = ypos/actualHeight * expectedHeight;

	gameManager->mouseX = virtualMouseX;
	gameManager->mouseY = virtualMouseY;
}

void reconfigureWindowSize(GLFWwindow* window, int width, int height) {
	// Adjust the window size to match the aspect ratio of the virtual screen size, which is 16:9
	SizePadding adjustedSizes = adjustToAspectRatio(width, height, kVirtualScreenSize.x, kVirtualScreenSize.y);

	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	int vpX = adjustedSizes.padLeft;
	int vpY = adjustedSizes.padTop;
	int vpWidth = width - (adjustedSizes.padLeft + adjustedSizes.padRight);
	int vpHeight = height - (adjustedSizes.padTop + adjustedSizes.padBottom);
	glViewport(vpX, vpY, vpWidth, vpHeight);
	if (vpWidth == 0 || vpHeight == 0) {
		return;
	}
	// Retrieve the pointer to gamemanager
	GameManager* gameManager = static_cast<GameManager*>(glfwGetWindowUserPointer(window));
	// Resize PostProcessing FBO
	if (gameManager->GetPostProcessor() != nullptr) {
		/*gameManager->GetPostProcessor()->Resize(adjustedSizes);*/
		ViewPortInfo newDstViewPortInfo = { vpX, vpY, vpWidth, vpHeight };
		gameManager->GetPostProcessor()->SetDstViewPort(newDstViewPortInfo);
	}
	//// Resize the scissor box handler
	//ScissorBoxHandler::GetInstance().SetActualWindowSizePadding(adjustedSizes);

	// memorize the actual window size padding
	Renderer::SetActualWindowSizePadding(adjustedSizes);

	//auto expectedWindowSizePadding = ScissorBoxHandler::GetInstance().GetExpectedWindowSizePadding();
	//auto actualWindowSizePadding = ScissorBoxHandler::GetInstance().GetActualWindowSizePadding();
	auto expectedWindowSizePadding = Renderer::GetExpectedWindowSizePadding();
	auto actualWindowSizePadding = Renderer::GetActualWindowSizePadding();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	reconfigureWindowSize(window, width, height);
}

int main()
{	
	// load configurations
	ConfigManager& configManager = ConfigManager::GetInstance();
	configManager.SetConfigPath("C:/Users/xiaod/resources/settings/display.txt");
	assert(configManager.LoadConfig() && "Failed to load configurations.");

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
	kWindowedModeSizePadding.padBottom = kWindowedModeSizePadding.padTop = kWindowedModeSizePadding.padLeft = kWindowedModeSizePadding.padRight = 0;
	kWindowedModeSize = glm::vec2(kWindowedModeSizePadding.width, kWindowedModeSizePadding.height);
	SetWindowSize(kVirtualScreenSize.x, kVirtualScreenSize.y);
	std::cout << "Resolution: " << mode->width << "x" << mode->height << std::endl;
	std::cout << "WINDOWED_MODE_SCREEN_WIDTH: " << kWindowedModeSize.x << " WINDOWED_MODE_SCREEN_HEIGHT: " << kWindowedModeSize.y << std::endl;
	std::cout << "FULL_SCREEN_WINDOW_WIDTH: " << kFullScreenSize.x << " FULL_SCREEN_WINDOW_HEIGHT: " << kFullScreenSize.y << std::endl;
	ScreenMode initialScreenMode = configManager.GetScreenMode();

	// By default, the screen mode is set to full screen mode
	SizePadding SCREEN_SIZE_PADDING = kFullScreenSizePadding;
	int SCREEN_WIDTH = SCREEN_SIZE_PADDING.GetPaddedWidth(), SCREEN_HEIGHT = SCREEN_SIZE_PADDING.GetPaddedHeight();
	GLFWwindow* window;
	if (initialScreenMode == ScreenMode::FULLSCREEN) {
		// Create the window
	    window = glfwCreateWindow(mode->width, mode->height, "DynastysDefender-ScrollsCurse", primaryMonitor, NULL);
		//// Update the screen size padding
		//int framebufferWidth, framebufferHeight;
		//glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
		//kFullScreenSizePadding = adjustToAspectRatio(framebufferWidth, framebufferHeight, kVirtualScreenSize.x, kVirtualScreenSize.y);
		//SCREEN_SIZE_PADDING = kFullScreenSizePadding;
		//SCREEN_WIDTH = SCREEN_SIZE_PADDING.GetPaddedWidth();
		//SCREEN_HEIGHT = SCREEN_SIZE_PADDING.GetPaddedHeight();
	}
	else {
		SCREEN_SIZE_PADDING = kWindowedModeSizePadding;
		// No padding for windowed mode
		SCREEN_WIDTH = kWindowedModeSize.x;
		SCREEN_HEIGHT = kWindowedModeSize.y;
		window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DynastysDefender-ScrollsCurse", NULL, NULL);
	}

	// Set the position of window to the center of the screen
	int windowPosX = (mode->width - SCREEN_WIDTH) / 2;
	int windowPosY = (mode->height - SCREEN_HEIGHT) / 2;
	glfwSetWindowPos(window, windowPosX, windowPosY);

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


	// OpenGL configuration
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// initialize game
	int GAME_AREA_WIDTH = kVirtualScreenSize.x;
	int GAME_AREA_HEIGHT = kVirtualScreenSize.y;
	assert(GAME_AREA_WIDTH > 0 && GAME_AREA_HEIGHT > 0 && "Invalid screen size!");
	// Clear the resources
	ResourceManager::GetInstance().Clear();
	//ScissorBoxHandler::GetInstance().SetExpectedWindowSizePadding(kVirtualScreenSizePadding);
	//ScissorBoxHandler::GetInstance().SetActualWindowSizePadding(kVirtualScreenSizePadding);
	Renderer::SetExpectedWindowSizePadding(kVirtualScreenSizePadding);
	Renderer::SetActualWindowSizePadding(SCREEN_SIZE_PADDING);
	GameManager gameManager(GAME_AREA_WIDTH, GAME_AREA_HEIGHT);
	gameManager.Init();
	std::cout << "Screen width: " << SCREEN_WIDTH << " Screen height: " << SCREEN_HEIGHT << std::endl;

	glfwSetWindowUserPointer(window, &gameManager);

	int framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	reconfigureWindowSize(window, framebufferWidth, framebufferHeight);


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
	/*gameManager.Init();*/
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

	//// Clear the resources
	//ResourceManager::GetInstance().Clear();

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
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Process input
		gameManager.ProcessInput(deltaTime);

		if (gameManager.targetScreenMode == ScreenMode::WINDOWED) {
			SCREEN_WIDTH = kWindowedModeSize.x;
			SCREEN_HEIGHT = kWindowedModeSize.y;
			int windowPosX = (mode->width - SCREEN_WIDTH) / 2;
			int windowPosY = (mode->height - SCREEN_HEIGHT) / 2;
			glfwSetWindowMonitor(window, NULL, windowPosX, windowPosY, SCREEN_WIDTH, SCREEN_HEIGHT, GLFW_DONT_CARE);
			gameManager.SetToTargetScreenMode();
		}
		else if (gameManager.targetScreenMode == ScreenMode::FULLSCREEN) {
			SCREEN_WIDTH = kFullScreenSize.x + kFullScreenSizePadding.padLeft + kFullScreenSizePadding.padRight;
			SCREEN_HEIGHT = kFullScreenSize.y + kFullScreenSizePadding.padTop + kFullScreenSizePadding.padBottom;
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			glfwSetWindowMonitor(window, primaryMonitor, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mode->refreshRate);
			gameManager.SetToTargetScreenMode();
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
