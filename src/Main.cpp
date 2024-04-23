// Main.cpp : Defines the entry point for the application.

#include "Main.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>

// window dimensions
glm::vec2 kOptionWindowSize = glm::vec2(3840, 2160);
glm::vec2 kWindowedModeSize = glm::vec2(3840, 2160);
glm::vec2 kFullScreenSize = glm::vec2(3840, 2160);
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
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	GameManager* gameManager = static_cast<GameManager*>(glfwGetWindowUserPointer(window));
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	gameManager->mouseX = static_cast<float>(mouseX);
	gameManager->mouseY = static_cast<float>(mouseY);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
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
	int WINDOWED_MODE_SCREEN_WIDTH = (mode->width * 7) / 8, FULL_SCREEN_WINDOW_WIDTH = mode->width, OPTION_WINDOW_WIDTH = (mode->width * 2) / 8;
	int WINDOWED_MODE_SCREEN_HEIGHT = (mode->height * 7) / 8, FULL_SCREEN_WINDOW_HEIGHT = mode->height, OPTION_WINDOW_HEIGHT = (mode->height * 2) / 8;
	kWindowedModeSize = glm::vec2(WINDOWED_MODE_SCREEN_WIDTH, WINDOWED_MODE_SCREEN_HEIGHT);
	kFullScreenSize = glm::vec2(FULL_SCREEN_WINDOW_WIDTH, FULL_SCREEN_WINDOW_HEIGHT);
	kOptionWindowSize = glm::vec2(OPTION_WINDOW_WIDTH, OPTION_WINDOW_HEIGHT);
	SetWindowSize(OPTION_WINDOW_WIDTH, OPTION_WINDOW_HEIGHT);
	std::cout << "Resolution: " << mode->width << "x" << mode->height << std::endl;
	std::cout << "SCREEN_WIDTH: " << std::endl;
	std::cout << "WINDOWED_MODE_SCREEN_WIDTH: " << WINDOWED_MODE_SCREEN_WIDTH << " WINDOWED_MODE_SCREEN_HEIGHT: " << WINDOWED_MODE_SCREEN_HEIGHT << std::endl;
	std::cout << "FULL_SCREEN_WINDOW_WIDTH: " << FULL_SCREEN_WINDOW_WIDTH << " FULL_SCREEN_WINDOW_HEIGHT: " << FULL_SCREEN_WINDOW_HEIGHT << std::endl;
	std::cout << "OPTION_WINDOW_WIDTH: " << OPTION_WINDOW_WIDTH << " OPTION_WINDOW_HEIGHT: " << OPTION_WINDOW_HEIGHT << std::endl;
	int SCREEN_WIDTH = OPTION_WINDOW_WIDTH, SCREEN_HEIGHT = OPTION_WINDOW_HEIGHT;
	// Create the window
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Puzzle Bobble", NULL, NULL);
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
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// initialize game
	GameManager gameManager(SCREEN_WIDTH, SCREEN_HEIGHT);
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
	gameManager.state = GameState::OPTION;
	gameManager.Init();
	//// Set the background color to (0.988f, 0.928f, 0.828f, 1.0f) which is a pale yellow
	//glClearColor(0.988f, 0.928f, 0.828f, 1.0f);
	// Start a loop that runs until the user chooses which mode to play
	while(!glfwWindowShouldClose(window)) {

		// Clear the color buffer with the specified clear color
		glClear(GL_COLOR_BUFFER_BIT);

		// Calculate delta time
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Process input
		gameManager.ProcessInput(deltaTime);

		// Draw the background
		gameManager.Render();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	if (gameManager.targetState == GameState::INITIAL){
		// Clear the resources
		ResourceManager::GetInstance().Clear();

		if (gameManager.screenMode == ScreenMode::WINDOWED) {
			SCREEN_WIDTH = WINDOWED_MODE_SCREEN_WIDTH;
			SCREEN_HEIGHT = WINDOWED_MODE_SCREEN_HEIGHT;
		}
		else if (gameManager.screenMode == ScreenMode::FULLSCREEN) {
			SCREEN_WIDTH = FULL_SCREEN_WINDOW_WIDTH;
			SCREEN_HEIGHT = FULL_SCREEN_WINDOW_HEIGHT;
		}
		// Set the position of window to the center of the screen
		int windowPosX = (mode->width - SCREEN_WIDTH) / 2;
		int windowPosY = (mode->height - SCREEN_HEIGHT) / 2;
	/*	glfwSetWindowPos(window, windowPosX, windowPosY);*/
		// OpenGL configuration
		// --------------------
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		/*glfwSetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);*/
		if (gameManager.screenMode == ScreenMode::FULLSCREEN) {
			glfwSetWindowMonitor(window, primaryMonitor, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mode->refreshRate);
		}
		else {
			glfwSetWindowMonitor(window, NULL, windowPosX, windowPosY, SCREEN_WIDTH, SCREEN_HEIGHT, GLFW_DONT_CARE);
		}
		std::cout << "refresh rate: " << mode->refreshRate << std::endl;
		std::cout << "screen size: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << std::endl;
		glfwSetWindowShouldClose(window, false);

		// Reconfigure the attibutes in the game that are affected by the change of screen size.
		SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		gameManager.width = SCREEN_WIDTH;
		gameManager.height = SCREEN_HEIGHT;


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

			// Update game state
			gameManager.Update(deltaTime);

			// Draw the background
			gameManager.Render();

			++GameManager::frameCount;

			// Swap the screen buffers
			glfwSwapBuffers(window);
		}
	}

	// Delete all resources.
	ResourceManager::GetInstance().Clear();

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	std::cout << "SCREEN_WIDTH: " << SCREEN_WIDTH << " SCREEN_HEIGHT: " << SCREEN_HEIGHT << std::endl;
	return EXIT_SUCCESS;
}
