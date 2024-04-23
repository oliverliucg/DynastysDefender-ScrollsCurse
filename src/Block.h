#pragma once

// Block is a class that define the postion and size of a block area on the screen.
// It is used to define the area of the game board and the area of scoreboard and other types of area

class Block{
public:
	Block(int x, int y, int width, int height);
	int getX();
	int getY();
	int getwidth();
	int getheight();
	void setX(int x);
	void setY(int y);
	void setwidth(int width);
	void setheight(int height);

private:
	int x;
	int y;
	int width;
	int height;

};