#pragma once
#include <glad/glad.h>
#include <glfw3.h>

#include <algorithm>
#include <bit>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <glm/glm.hpp>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "Arrow.h"
#include "Bubble.h"
#include "Button.h"
#include "CJTextRenderer.h"
#include "Capsule.h"
#include "CircleRenderer.h"
#include "ColorRenderer.h"
#include "ConfigManager.h"
#include "ExplosionSystem.h"
#include "GameBoard.h"
#include "GameCharacter.h"
#include "LineRenderer.h"
#include "Page.h"
#include "PartialTextureRenderer.h"
#include "PostProcessor.h"
#include "PowerUp.h"
#include "RayRenderer.h"
#include "ResourceManager.h"
#include "ScissorBoxHandler.h"
#include "Scroll.h"
#include "ShadowTrailSystem.h"
#include "Shooter.h"
#include "SoundEngine.h"
#include "SpriteDynamicRenderer.h"
#include "SpriteRenderer.h"
#include "Text.h"
#include "TextRenderer.h"
#include "Timer.h"
#include "WesternTextRenderer.h"

enum class GameState {
  UNDEFINED,
  PRELOAD,
  SPLASH_SCREEN,
  INTRO,
  INITIAL,
  STORY,
  STORY_END,
  ACTIVE,
  CONTROL,
  DIFFICULTY_SETTINGS,
  DISPLAY_SETTINGS,
  LANGUAGE_PREFERENCE,
  MENU,
  PREPARING,
  WIN,
  LOSE,
  EXIT
};

// Transition between states
enum class TransitionState { START, TRANSITION, END };

struct GameLevel {
  // Num of colors
  int numColors{};
  // The number of bubbles that are generated at the beginning of the game
  int numInitialBubbles{};
  // The least distance between the bubble and the bottom of the game board
  float minDistanceToBottom{};
  // The parameters for the ellipse centered at the shooter and the new bubble
  // should be generated outside of it.
  float minHorizontalDistanceToShooter{}, minVerticalDistanceToShooter{};
  // Probability that a new bubble is generated adjacent to the most recently
  // added bubble
  float probabilityNewBubbleIsNeighborOfLastAdded{};
  // Probability that a new bubble is generated as a neighbor of an existing
  // bubble
  float probabilityNewBubbleIsNeighborOfBubble{};
  // Probability that a new bubble is generated as a neighbor of an existing
  // bubble of the same color
  float probabilityNewBubbleIsNeighborOfBubbleOfSameColor{};
  // possibility that the new bubble is new color compared to the last bubble
  float probabilityNewBubbleIsNewColor{};
  // time interval for narrowing the game board vertically
  float narrowingTimeInterval{};
};

struct GameStateSnapshot {
  glm::vec2 scrollCenter;
  GameBoardState gameBoardState;
  ScreenMode screenMode;
  bool isScrollIconAllowed;
  float storyPageTextOffset;
};

class GameManager {
 public:
  GameState state{GameState::INITIAL}, lastState{GameState::UNDEFINED},
      targetState{GameState::UNDEFINED};
  TransitionState transitionState{TransitionState::START};
  Difficulty difficulty{Difficulty::EASY};
  ScreenMode screenMode{ScreenMode::WINDOWED};
  ScreenMode targetScreenMode{ScreenMode::UNDEFINED};
  Language language{Language::ENGLISH};
  bool keys[1024] = {false};
  bool keysLocked[1024] = {false};
  bool focused{true};
  bool leftMousePressed{false};
  bool isReadyToDrag{true};
  bool isDragging{false};
  bool gameArenaShaking{false};
  float scrollYOffset{0.f};
  float scrollSensitivity{25.f};
  float mouseX{0.f}, mouseY{0.f}, mouseLastX{0.f}, mouseLastY{0.f};
  float width, height;
  int level{1};
  bool isLevelFailed{false};
  GameLevel gameLevel;
  int64_t score{0};
  int comboCount{0};
  bool hideDefaultMouseCursor{false};

  GameManager(unsigned int width, unsigned int height);
  ~GameManager();
  std::shared_ptr<PostProcessor> GetPostProcessor() { return postProcessor; }
  // Set to the target screen mode of the game.
  void SetToTargetScreenMode();

  void PreLoad();
  void Init();
  void LoadSounds();
  void LoadStreams();
  void ProcessInput(float dt);
  void Update(float dt);
  void Render();
  GameStateSnapshot PrepareToReload();
  void Reload(const GameStateSnapshot& snapshot);

 private:
  std::shared_ptr<SpriteRenderer> spriteRenderer;
  std::shared_ptr<SpriteDynamicRenderer> spriteDynamicRenderer;
  std::shared_ptr<PartialTextureRenderer> partialTextureRenderer;
  std::shared_ptr<ColorRenderer> colorRenderer;
  std::shared_ptr<CircleRenderer> circleRenderer;
  std::shared_ptr<RayRenderer> rayRenderer;
  std::shared_ptr<LineRenderer> lineRenderer;
  std::unordered_map<Language, std::shared_ptr<TextRenderer>> textRenderers;
  std::shared_ptr<PostProcessor> postProcessor;
  std::shared_ptr<Timer> timer;

  // Gameboard
  std::unique_ptr<GameBoard> gameBoard;

  // Scroll
  std::unique_ptr<Scroll> scroll;

  // Shooter
  std::unique_ptr<Shooter> shooter;

  // PowerUps
  std::unique_ptr<PowerUp> powerUp;

  // Particles
  std::unique_ptr<ShadowTrailSystem> shadowTrailSystem;
  std::unique_ptr<ExplosionSystem> explosionSystem;

  // Characters
  std::unordered_map<std::string, std::shared_ptr<GameCharacter>>
      gameCharacters;

  // Bubbbles that are moving.
  std::unordered_map<int, std::unique_ptr<Bubble>> moves;

  // Bubbles that are static.
  std::unordered_map<int, std::unique_ptr<Bubble>> statics;

  // Bubbles that are falling.
  std::unordered_map<int, std::unique_ptr<Bubble>> fallings;

  // Bubbles that are exploding.
  std::unordered_map<int, std::unique_ptr<Bubble>> explodings;

  // Stores individual scores gained during game play.
  std::queue<int> scoreIncrements;

  // Stores the scores' text objects and their time to start fading out.
  std::vector<std::pair<std::shared_ptr<Text>, float>> scoreIncrementTexts;

  // Number of score increments to be reflected on the screen
  int numOfScoreIncrementsReady{0};

  // Scale of the score increment.
  int scoreIncrementScale{1};

  // Arrows
  std::vector<std::shared_ptr<Arrow>> arrows;

  // Texts
  std::unordered_map<std::string, std::shared_ptr<Text>> texts;

  // Buttons
  std::unordered_map<std::string, std::shared_ptr<Button>> buttons;

  // Pages
  std::unordered_map<std::string, std::unique_ptr<Page>> pages;

  // Active page
  std::string activePage{""};

  // Free slots on the game board.
  std::vector<glm::vec2> freeSlots;

  // Count of bubbles of each color.
  std::unordered_map<Color, int> colorCount;

  // Weight of bubbles of each color.
  std::unordered_map<Color, float> colorWeight;

  // original positions for shaking.
  std::unordered_map<std::string, glm::vec2> originalPositionsForShaking;

  // objects that gradually become transparent. strucutre: <object name,
  // <object, speedToBeTransparent>>
  std::map<std::string, std::pair<std::shared_ptr<GameObject>, float>>
      graduallyTransparentObjects;

  // objects that gradually become opaque. strucutre: <object name, <object,
  // speedToBeOpaque>>
  std::map<std::string, std::pair<std::shared_ptr<GameObject>, float>>
      graduallyOpaqueObjects;

  // Get total number of game levels.
  int GetNumGameLevels();

  // Get bubble number for each level range.
  int GetBubbleNumForLevel();

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

  // Set the screen mode of the game.
  void SetScreenMode(ScreenMode newScreenMode);

  // Get the screen mode of the game.
  ScreenMode GetScreenMode() const;

  // Set the language of the game.
  void SetLanguage(Language newLanguage);

  // Get the language of the game.
  Language GetLanguage() const { return language; }

  // Set the difficulty of the game.
  void SetDifficulty(Difficulty newDifficulty);

  // Get the difficulty of the game.
  Difficulty GetDifficulty() const;

  // Load the font based on the language.
  void LoadTextRenderer();

  // Re-load the font based on the language.
  void ReloadTextRenderer();

  // Load the control characters that are used in the game. For example,
  // descenders like 'g', 'j', 'p', 'q', and 'y', that could be used to
  // determine the height of the text. Also load the space character that could
  // be used for tabulation.
  void LoadCommonCharacters();

  // Load text based from resource file.
  void LoadTexts();

  // Reload text based from resource file.
  void ReloadTexts();

  // Load buttons based from resource file.
  void LoadButtons();

  // Reload buttons based from resource file.
  void ReloadButtons();

  // Get the text renderer of the game.
  std::shared_ptr<TextRenderer> GetTextRenderer();

  // Set the transition state of the game.
  void SetTransitionState(TransitionState newTransitionState);

  // Get the transition state of the game.
  TransitionState GetTransitionState();

  // Set Scroll state
  void SetScrollState(ScrollState newScrollState);

  // Get Scroll state
  ScrollState GetScrollState();

  // Calculate the offsets when scroll is shaking.
  glm::vec2 CalculateScrollShakingOffsets(bool isScrollVibrating = true);

  // Check if the bubble collides with the existing static bubbles
  std::vector<int> IsCollidingWithStaticBubbles(
      std::unique_ptr<Bubble>& bubble);

  // Check if the bubble is neighbor with the given free slot.
  bool IsNeighbor(glm::vec2 bubbleCenter, glm::vec2 slotCenter,
                  float absError = 0.5);

  // Get common neighbor free slots among the given bubbles.
  std::vector<glm::vec2> GetCommonFreeSlots(
      std::vector<std::unique_ptr<Bubble>>& bubbles,
      std::vector<glm::vec2> candidateFreeSlots);

  // Get common neighbor free slots among the given bubbles.
  std::vector<glm::vec2> GetCommonFreeSlots(
      std::vector<int>& bubbleIds, std::vector<glm::vec2> candidateFreeSlots);

  // Given the bubble's color, get the weight of a free slot. The weight is
  // calculated by counting the number of neighbors. If a neighbor has the same
  // color, the weight is 1. Otherwise, the weight is 0.1.
  float GetFreeSlotWeight(glm::vec3 color, glm::vec2 slotCenter);

  // Get the unique id of all neighbor static bubbles of the given bubble.
  std::vector<int> GetNeighborIds(std::unique_ptr<Bubble>& bubble,
                                  float absError = 0.5f);

  // Get the unique id of all neighbor static bubbles of the given group of
  // bubbles.
  std::vector<int> GetNeighborIds(std::vector<Bubble*>& bubbles);

  // Get the unique id of all neighbor static bubbles of the given group of
  // bubbles.
  std::vector<int> GetNeighborIds(
      std::unordered_map<int, std::unique_ptr<Bubble>>& bubbles);

  // Check if a poistion is at the upper boundary of the game board.
  bool IsAtUpperBoundary(glm::vec2 pos);

  // Find all the bubbles that have no path to the top of the game board.
  std::vector<int> FindAllFallingBubbles();

  // Find a group of bubbles that are connected to the given bubble and have the
  // same color as the given bubble.
  std::vector<int> FindConnectedBubblesOfSameColor(int bubbleId);

  // Find all the bubbles that have path to the given bubble ids.
  std::vector<int> FindConnectedBubbles(std::vector<int>& bubbleIds);

  // Find all the statics bubbles that are at the upper boundary of the game
  // board and close to the given bubble.
  std::vector<int> FindCloseUpperBubbles(std::unique_ptr<Bubble>& bubble);

  // Find the left bubble of the given bubble in the same row.
  int FindLeftBubble(std::unique_ptr<Bubble>& bubble);

  // Find the right bubble of the given bubble in the same row.
  int FindRightBubble(std::unique_ptr<Bubble>& bubble);

  // Fine tune the position of the collided moving bubble to a free slot
  // neighboring a neighbor of the colliding static bubble.
  bool FineTuneToNeighbor(int bubbleId, int staticBubbleId);

  // Fine tune the position of the collided moving bubble to a free slot
  // neighboring both the colliding static bubble and a close static bubble.
  bool FineTuneToClose(int bubbleId, int staticBubbleId);

  // Fine tune the position of the collided moving bubble to a free slot
  // neighboring a close static bubble at the upper boundary of the game board.
  bool FineTuneToCloseUpper(int bubbleId);

  // Adjusts the position of a moving bubble to ensure it correctly occupies a
  // free slot after collision.
  //
  // This function fine-tunes the position of a bubble when it collides with
  // another or reaches the boundary:
  // 1. When adjacent to a static bubble:
  //    - The function aligns the moving bubble into a free neighboring slot.
  //    - The alignment is based on the angle with the static bubble's right
  //    neighbor, which must be a multiple of 30 degrees.
  // 2. When the bubble reaches the upper boundary of the game board:
  //    - The function places it in a slot along the boundary.
  //    - The distance to the nearest slot is determined by one of the following
  //    formulas, where n is a non-negative integer (n >= 0):
  //      a. 2*sqrt(2) + n*4
  //      b. 2*sqrt(3) + n*4
  //      c. n*4
  //
  // Returns true if fine tunning succeessfully updates the bubble's position.
  bool FineTuneToCorrectPosition(int bubbleId);

  // Helper function of IsConnectedToTop
  bool IsConnectedToTopHelper(std::unique_ptr<Bubble>& bubble, bool* visited,
                              bool* isConnectToTop);

  // Check if the given bubble is connected to the top of the game board.
  bool IsConnectedToTop(std::unique_ptr<Bubble>& bubble);

  // Get the common free slots of the given two free slot vectors.
  std::vector<glm::vec2> GetCommonFreeSlots(std::vector<glm::vec2> freeSlots1,
                                            std::vector<glm::vec2> freeSlots2);

  // Check if a free slot center is a neighbor of static bubbles.
  bool IsNeighborOfStaticBubbles(glm::vec2 freeSlotCenter);

  // Get potential neighbor free slots of the given bubble.
  std::vector<glm::vec2> GetPotentialNeighborFreeSlots(glm::vec2 bubbleCenter);

  // Update the free slots of the game board after inserting a new bubble.
  void UpdateFreeSlots(std::unique_ptr<Bubble>& bubble,
                       float minDistanceToBottom = 0.f,
                       float minHorizontalDistanceToShooter = 0.f,
                       float minVerticalDistanceToShooter = 0.f);

  // Generate random static bubbles on the free slots of the game board.
  void GenerateRandomStaticBubblesHelper(GameLevel gameLevel);

  // Generate random static bubbles on the game board.
  void GenerateRandomStaticBubbles();

  // Get the time interval for narrowing the game board vertically in the
  // current game level.
  float GetNarrowingTimeInterval();

  // Determine the color of the next bubble.
  glm::vec4 GetNextBubbleColor();

  // Adjust the horizontal position of the buttons.
  void AdjustButtonsHorizontalPosition(
      const std::vector<std::string>& buttonName, float interval = 0.f);

  // Check if it is failed to pass the current level.
  bool IsLevelFailed();

  // Create clickable option units with the image of bubble as the icon.
  std::shared_ptr<OptionUnit> CreateClickableOptionUnit(
      const std::string& name, const std::u32string& text,
      std::shared_ptr<TextRenderer> textRenderer = nullptr);

  // Reset the state of each game character.
  void ResetGameCharacters();

  // Calculate the score based on the number of bubbles, the bubble size, the
  // bubble type (exploding or falling), the game level, and the timed used in
  // the current round.
  int CalculateScore(int numBubbles, float bubbleRadius, BubbleState bubbleType,
                     float timeUsed);

  void AddScoreIncrementText(int scoreIncrement, glm::vec2 scorePosition);

  // Increase the score by the given value.
  void IncreaseScore(const int64_t value);

  // Decrease the score by the given value.
  void DecreaseScore(const int64_t value);

  // Reset the score to zero or the given value.
  void ResetScore(int64_t value = 0);

  void ClearResources();
};
