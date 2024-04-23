#pragma once
#include "GameObject.h"

// State of the game board
enum class GameBoardState {
	INGAME,
	ACTIVE,
	INACTIVE,
};

// Color of the game board
enum class GameBoardColor {
	ORIGINAL,
	GRAY,
};

// A hash map of the colors of the game board
static std::unordered_map<GameBoardColor, glm::vec3> GameBoardColorMap = {
	{GameBoardColor::ORIGINAL, glm::vec3(1.0f, 1.0f, 1.0f)},
	{GameBoardColor::GRAY, glm::vec3(0.20079f, 0.20902f, 0.22973f)},
};

// GameBoard is a class that represents the game board for the game Puzzle Bobble.
class GameBoard : public GameObject {
public:
	// Constructs a new GameBoard instance. Requires a position vector, size vector, and color vector.
	GameBoard(glm::vec2 pos, glm::vec2 size, glm::vec4 color, Texture2D texture);
	~GameBoard();
	// Getters and setters for the boundaries of the game board.
	glm::vec4 GetBoundaries();
	glm::vec4 GetValidBoundaries();
	// Set the valid boundaries of the game board.
	void SetValidBoundaries(glm::vec4 validBoundaries);

	glm::vec2 GetValidPosition();

	void SetValidPosition(glm::vec2 validPosition);

	void SetPosition(glm::vec2 pos);

	glm::vec2 GetValidSize();

	void SetValidSize(glm::vec2 validSize);

	// Getters and setters for the state of the game board.
	GameBoardState GetState();
	void SetState(GameBoardState state);

	// Update the color of the game board based on the color of ray being displayed.
	void UpdateColor(glm::vec3 rayColor);

private:
	// State of the game board
	GameBoardState state;

	// Boudaries of the game board.
	glm::vec4 boundaries;

	// Position of valid playing area.
	glm::vec2 validPosition;

	// Size of valid playing area.
	glm::vec2 validSize;

};