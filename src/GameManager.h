#pragma once
#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <random> 
#include <chrono> 
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <utility>
#include <bit>
#include <cstdint>
#include <memory>

#include "Bubble.h"
#include "Shooter.h"
#include "Arrow.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "SpriteDynamicRenderer.h"
#include "PartialTextureRenderer.h"
#include "ColorRenderer.h"
#include "PostProcessor.h"
#include "GameBoard.h"
#include "ShadowTrailSystem.h"
#include "ExplosionSystem.h"
#include "TextRenderer.h"
#include "RayRenderer.h"
#include "LineRenderer.h"
#include "CircleRenderer.h"
#include "GameCharacter.h"
#include "ScissorBoxHandler.h"
#include "Scroll.h"
#include "Text.h"
#include "Capsule.h"
#include "Button.h"
#include "Page.h"
#include "Timer.h"

enum class GameState {
	UNDEFINED,
	OPTION,
	INITIAL,
	STORY,
	STORY_END,
	ACTIVE,
	CONTROL,
	DISPLAY_SETTINGS,
	MENU,
	PREPARING,
	WIN,
	LOSE,
	EXIT
};

enum class ScreenMode {
	UNDEFINED,
	WINDOWED,
	FULLSCREEN
};

// Transition between states
enum class TransitionState {
	START,
	TRANSITION,
	END
};


// GameState to string
static std::unordered_map<GameState, std::string> GameStateMap = {
	{GameState::INITIAL, "INITIAL"},
	{GameState::STORY, "STORY"},
	{GameState::ACTIVE, "ACTIVE"},
	{GameState::CONTROL, "CONTROL"},
	{GameState::MENU, "MENU"},
	{GameState::PREPARING, "PREPARING"},
	{GameState::WIN, "WIN"},
	{GameState::LOSE, "LOSE"},
	{GameState::STORY_END, "STORY_END"},
	{GameState::EXIT, "EXIT"},
	{GameState::UNDEFINED, "UNDEFINED"},
};

struct GameLevel {
	// Num of colors
	int numColors;
	// The number of bubbles that are generated at the beginning of the game
	int numInitialBubbles;
	// The maximum depth that the initial bubbles can reach
	float maxInitialBubbleDepth;
	// Probability that a new bubble is generated adjacent to the most recently added bubble
	float probabilityNewBubbleIsNeighborOfLastAdded;
	// Probability that a new bubble is generated as a neighbor of an existing bubble
	float probabilityNewBubbleIsNeighborOfBubble;
	// Probability that a new bubble is generated as a neighbor of an existing bubble of the same color
	float probabilityNewBubbleIsNeighborOfBubbleOfSameColor;
};

class GameManager {
public:
	GameState state, lastState, targetState;
	TransitionState transitionState;
	ScreenMode screenMode, targetScreenMode;
	bool keys[1024];
	bool keysLocked[1024];
	bool leftMousePressed;
	bool isReadyToDrag;
	bool isDragging;
	bool gameArenaShaking;
	float scrollYOffset;
	float scrollSensitivity;
	float mouseX, mouseY, mouseLastX, mouseLastY;
	float width, height;
	int level;
	GameManager(unsigned int width, unsigned int height);
	~GameManager();
	std::shared_ptr<PostProcessor> GetPostProcessor() { return postProcessor; }
	void Init();
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
private:
	std::shared_ptr<SpriteRenderer> spriteRenderer;
	std::shared_ptr<SpriteDynamicRenderer> spriteDynamicRenderer;
	std::shared_ptr<PartialTextureRenderer> partialTextureRenderer;
	std::shared_ptr<ColorRenderer> colorRenderer;
	std::shared_ptr<CircleRenderer> circleRenderer;
	std::shared_ptr<RayRenderer> rayRenderer;
	std::shared_ptr<LineRenderer> lineRenderer;
	std::shared_ptr<TextRenderer> textRenderer;
	std::shared_ptr<TextRenderer> textRenderer2;
	std::shared_ptr<PostProcessor> postProcessor;
	std::shared_ptr<Timer> timer;

	// Gameboard
	std::unique_ptr<GameBoard> gameBoard;

	// Scroll
	std::unique_ptr<Scroll> scroll;

	// Shooter
	std::unique_ptr<Shooter> shooter;

	// Particles
	std::unique_ptr<ShadowTrailSystem> shadowTrailSystem;
	std::unique_ptr<ExplosionSystem> explosionSystem;

	// Characters
	std::unordered_map<std::string, std::shared_ptr<GameCharacter>> gameCharacters;

	// Bubbbles that are moving.
	std::unordered_map<int, std::unique_ptr<Bubble>> moves;

	// Bubbles that are static.
	std::unordered_map<int, std::unique_ptr<Bubble>> statics;

	// Bubbles that are falling.
	std::unordered_map<int, std::unique_ptr<Bubble>> fallings;

	// Bubbles that are exploding.
	std::unordered_map<int, std::unique_ptr<Bubble>> explodings;

	// Arrows
	std::vector<std::shared_ptr<Arrow> > arrows;

	// Texts
	std::unordered_map<std::string, std::shared_ptr<Text>> texts;

	// Buttons
	std::unordered_map<std::string, std::shared_ptr<Button>> buttons;

	// Pages
	std::unordered_map<std::string, std::unique_ptr<Page>> pages;

	// Active page
	std::string activePage;

	// Free slots on the game board.
	std::vector<glm::vec2> freeSlots;

	// Count of bubbles of each color.
	std::unordered_map<Color, int> colorCount;

	// original positions for shaking.
	std::unordered_map<std::string, glm::vec2> originalPositionsForShaking;

	// objects that gradually become transparent. strucutre: <object name, <object, speedToBeTransparent>>
	std::map<std::string, std::pair<std::shared_ptr<GameObject>, float> > graduallyTransparentObjects;

	// objects that gradually become opaque. strucutre: <object name, <object, speedToBeOpaque>>
	std::map<std::string, std::pair<std::shared_ptr<GameObject>, float> > graduallyOpaqueObjects;

	// total number of game levels
	const int numGameLevels = 2;

	// Get the page name for a given game state.
	std::string GetPageName(GameState gameState);

	// Set the state of the game.
	void SetState(GameState newState);

	// Set to the target state of the game.
	void SetToTargetState();

	// Get the state of the game.
	GameState GetState();

	// Go to the state of the game.
	void GoToState(GameState newState);

	// Go to screen mode.
	void GoToScreenMode(ScreenMode newScreenMode);

	// Get the screen mode of the game.
	ScreenMode GetScreenMode();

	// Set the screen mode of the game.
	void SetScreenMode(ScreenMode newScreenMode);

	// Set to the target screen mode of the game.
	void SetToTargetScreenMode();

	// Set the transition state of the game.
	void SetTransitionState(TransitionState newTransitionState);

	// Get the transition state of the game.
	TransitionState GetTransitionState();

	// Set Scroll state
	void SetScrollState(ScrollState newScrollState);

	// Get Scroll state
	ScrollState GetScrollState();

	// Check if the bubble collides with the existing static bubbles
	std::vector<int> IsCollidingWithStaticBubbles(std::unique_ptr<Bubble>& bubble);

	// Check if the bubble is neighbor with the given free slot.
	bool IsNeighbor(glm::vec2 bubbleCenter, glm::vec2 slotCenter, float absError = 1e-2);

	// Get common neighbor free slots among the given bubbles.
	std::vector<glm::vec2> GetCommonFreeSlots(std::vector<std::unique_ptr<Bubble>>& bubbles, std::vector<glm::vec2> candidateFreeSlots);

	// Get common neighbor free slots among the given bubbles.
	std::vector<glm::vec2> GetCommonFreeSlots(std::vector<int>& bubbleIds, std::vector<glm::vec2> candidateFreeSlots);

	// Given the bubble's color, get the weight of a free slot. The weight is calculated by counting the number of neighbors. If a neighbor has the same color, the weight is 1. Otherwise, the weight is 0.1.
	float GetFreeSlotWeight(glm::vec3 color, glm::vec2 slotCenter);

	// Get the unique id of all neighbor static bubbles of the given bubble.
	std::vector<int> GetNeighborIds(std::unique_ptr<Bubble>& bubble, float absError = 1e-2);

	// Get the unique id of all neighbor static bubbles of the given group of bubbles.
	std::vector<int> GetNeighborIds(std::vector<Bubble*>& bubbles);

	// Get the unique id of all neighbor static bubbles of the given group of bubbles.
	std::vector<int> GetNeighborIds(std::unordered_map<int, std::unique_ptr<Bubble> >& bubbles);

	// Check if a poistion is at the upper boundary of the game board.
	bool IsAtUpperBoundary(glm::vec2 pos);

	// Find all the bubbles that have no path to the top of the game board.
	std::vector<int> FindAllFallingBubbles();
	
	// Find a group of bubbles that are connected to the given bubble and have the same color as the given bubble.
	std::vector<int> FindConnectedBubblesOfSameColor(int bubbleId);

	// Find all the bubbles that have path to the given bubble ids.
	std::vector<int> FindConnectedBubbles(std::vector<int>& bubbleIds);

	// Find all the statics bubbles that are at the upper boundary of the game board and close to the given bubble.
	std::vector<int> FindCloseUpperBubbles(std::unique_ptr<Bubble>& bubble);
 
	// Fine tune the position of the collided moving bubble to a free slot neighboring a neighbor of the colliding static bubble.
	bool FineTuneToNeighbor(int bubbleId, int staticBubbleId);

	// Fine tune the position of the collided moving bubble to a free slot neighboring both the colliding static bubble and a close static bubble.
	bool FineTuneToClose(int bubbleId, int staticBubbleId);

	// Fine tune the position of the collided moving bubble to a free slot neighboring a close static bubble at the upper boundary of the game board.
	bool FineTuneToCloseUpper(int bubbleId);

	// Helper function of IsConnectedToTop
	bool IsConnectedToTopHelper(std::unique_ptr<Bubble>& bubble, bool* visited, bool* isConnectToTop);

	// Check if the given bubble is connected to the top of the game board.
	bool IsConnectedToTop(std::unique_ptr<Bubble>& bubble);

	// Get the common free slots of the given two free slot vectors.
	std::vector<glm::vec2> GetCommonFreeSlots(std::vector<glm::vec2> freeSlots1, std::vector<glm::vec2> freeSlots2);

	// Check if a free slot center is a neighbor of static bubbles.
	bool IsNeighborOfStaticBubbles(glm::vec2 freeSlotCenter);

	// Get potential neighbor free slots of the given bubble.
	std::vector<glm::vec2> GetPotentialNeighborFreeSlots(glm::vec2 bubbleCenter);

	// Update the free slots of the game board after inserting a new bubble.
	void UpdateFreeSlots(std::unique_ptr<Bubble>& bubble);

	// Generate random static bubbles on the free slots of the game board.
	void GenerateRandomStaticBubblesHelper(GameLevel gameLevel);

	// Generate random static bubbles on the game board.
	void GenerateRandomStaticBubbles();

	// Get the time interval for narrowing the game board vertically in the current game level.
	float GetNarrowingTimeInterval();

	// Determine the color of the next bubble.
	glm::vec4 GetNextBubbleColor();
	
	// Adjust the horizontal position of the buttons.
	void AdjustButtonsHorizontalPosition(const std::vector<std::string>& buttonName, float interval = 0.f);

	// Check if it is failed to pass the current level.
	bool IsLevelFailed();

	// Create clickable option units with the image of bubble as the icon.
	std::shared_ptr<OptionUnit> CreateClickableOptionUnit(const std::string& name, const std::string& text);

};
