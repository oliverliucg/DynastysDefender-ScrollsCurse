#include "GameBoard.h"


GameBoard::GameBoard(glm::vec2 pos, glm::vec2 size, glm::vec4 color, Texture2D sprite) : 
	GameObject(pos, size, 0.0f, glm::vec2(0.5f, 0.5f), glm::vec2(0.0f, 0.0f), color, sprite), state(GameBoardState::INACTIVE),
	boundaries(pos.x, pos.y, pos.x+size.x, pos.y+size.y), validPosition(pos), validSize(size) {}

GameBoard::~GameBoard() {}

glm::vec4 GameBoard::GetBoundaries() {
	return boundaries;
}

glm::vec4 GameBoard::GetValidBoundaries() {
	glm::vec4 validBoundaries;
	validBoundaries.x = validPosition.x;
	validBoundaries.y = validPosition.y;
	validBoundaries.z = validPosition.x + validSize.x;
	validBoundaries.w = validPosition.y + validSize.y;
	return validBoundaries;
}

glm::vec2 GameBoard::GetValidPosition() {
	return validPosition;
}

void GameBoard::SetValidPosition(glm::vec2 validPosition) {
	this->validPosition = validPosition;
}

void GameBoard::SetPosition(glm::vec2 pos) {
	GameObject::SetPosition(pos);
	boundaries.x = pos.x;
	boundaries.y = pos.y;
	boundaries.z = pos.x + size.x;
	boundaries.w = pos.y + size.y;
}

glm::vec2 GameBoard::GetValidSize() {
	return validSize;
}

void GameBoard::SetValidSize(glm::vec2 validSize) {
	this->validSize = validSize;
}

GameBoardState GameBoard::GetState() {
	return state;
}

void GameBoard::UpdateColor(glm::vec3 rayColor) {
	// If the ray color is Blue, Purple, and red, then the color of the game board should be white to make intense contrast.
	if (areFloatsEqual(rayColor, colorMap[Color::Blue]) || areFloatsEqual(rayColor, colorMap[Color::Purple]) || areFloatsEqual(rayColor, colorMap[Color::Red])) {
		SetColor(glm::vec4(GameBoardColorMap[GameBoardColor::ORIGINAL], 0.9f));
	}
	else {
		SetColor(glm::vec4(GameBoardColorMap[GameBoardColor::GRAY], 0.9f));
	}
}

void GameBoard::SetState(GameBoardState state) {
	this->state = state;
	// Set the color based on the state of the game board.
	switch (state) {		
	case GameBoardState::INGAME:
		SetColor(glm::vec4(GameBoardColorMap[GameBoardColor::GRAY], 0.9f));
		break;
	default:
		SetColor(glm::vec4(GameBoardColorMap[GameBoardColor::ORIGINAL], 0.9f));
		break;
	}
}