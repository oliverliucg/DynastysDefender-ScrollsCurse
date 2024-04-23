#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
// Board is a base class for all types of board like game board, score board, etc.
// It stores the board's width and height, and the board's background color, and the position of the board.
// It also has a pure virtual function draw() which is used to draw the board.

class Board{
protected:
	glm::vec2 pos;
	glm::vec2 size;
	glm::vec4 color;
public:
	Board(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
	virtual void draw() = 0;

};
