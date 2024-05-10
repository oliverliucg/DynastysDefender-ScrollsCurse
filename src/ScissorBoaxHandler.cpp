#include "ScissorBoxHandler.h"
#include <iostream>

ScissorBoxHandler::ScissorBoxHandler() : scissor_box_({ 0, 0, INT_MAX, INT_MAX }), prev_scissor_box_({ 0, 0, INT_MAX, INT_MAX }), scissor_test_enabled_(false), 
expected_window_size_padding_(), actual_window_size_padding_() {}

ScissorBoxHandler::~ScissorBoxHandler() {}


void ScissorBoxHandler::SetExpectedWindowSizePadding(SizePadding sizePadding)
{
	expected_window_size_padding_ = sizePadding;
}

void ScissorBoxHandler::SetActualWindowSizePadding(SizePadding sizePadding)
{
	actual_window_size_padding_ = sizePadding;
}

int ScissorBoxHandler::GetExpectedWindowWidth() const
{
	return expected_window_size_padding_.width;
}

int ScissorBoxHandler::GetExpectedWindowHeight() const
{
	return expected_window_size_padding_.height;
}

int ScissorBoxHandler::GetActualWindowWidth() const
{
	return actual_window_size_padding_.width;
}

int ScissorBoxHandler::GetActualWindowHeight() const
{
	return actual_window_size_padding_.height;
}

SizePadding ScissorBoxHandler::GetExpectedWindowSizePadding() const
{
	return expected_window_size_padding_;
}

SizePadding ScissorBoxHandler::GetActualWindowSizePadding() const
{
	return actual_window_size_padding_;
}

void ScissorBoxHandler::SetScissorBox(GLint x, GLint y, GLsizei width, GLsizei height)
{
	MemorizeScissorBox();
	scissor_box_ = { x, y, width, height };
	//glScissor(x, y, width, height);
	GLint actualX = x * actual_window_size_padding_.width / expected_window_size_padding_.width;
	GLint actualY = y * actual_window_size_padding_.height / expected_window_size_padding_.height;
	GLsizei actualWidth = width * actual_window_size_padding_.width / expected_window_size_padding_.width;
	GLsizei actualHeight = height * actual_window_size_padding_.height / expected_window_size_padding_.height;
	//actualX += actual_window_size_padding_.padLeft;
	//actualY += actual_window_size_padding_.padBottom;
	glScissor(actualX + actual_window_size_padding_.padLeft, actualY + actual_window_size_padding_.padBottom, actualWidth, actualHeight);
	/*std::cout << "actualX: " << actualX << " actualY: " << actualY << " actualWidth: " << actualWidth << " actualHeight: " << actualHeight << std::endl;
	std::cout << "actualX + padLeft: " << actualX + actual_window_size_padding_.padLeft << " actualY+padBottom: " << actualY + actual_window_size_padding_.padBottom << std::endl;*/
	/*return { actualX, actualY, actualWidth, actualHeight };*/
}

void ScissorBoxHandler::SetScissorBox(const ScissorBox& scissorBox)
{
	SetScissorBox(scissorBox.x, scissorBox.y, scissorBox.width, scissorBox.height);
}

void ScissorBoxHandler::SetIntersectedScissorBox(GLint x, GLint y, GLsizei width, GLsizei height)
{
	ScissorBox intersection;
	intersection.x = std::max(scissor_box_.x, x);
	intersection.y = std::max(scissor_box_.y, y);
	intersection.width = std::min(scissor_box_.x + scissor_box_.width, x + width) - intersection.x;
	intersection.height = std::min(scissor_box_.y + scissor_box_.height, y + height) - intersection.y;
	//std::cout << "input: " << x << " " << y << " " << width << " " << height << std::endl;
	//std::cout << "scissor_box: " << scissor_box_.x << " " << scissor_box_.y << " " << scissor_box_.width << " " << scissor_box_.height << std::endl;
	//std::cout << "intersection: " << intersection.x << " " << intersection.y << " " << intersection.width << " " << intersection.height << std::endl;
	intersection.width = std::max(0, intersection.width);
	intersection.height = std::max(0, intersection.height);
	/*assert(intersection.width >= 0 && intersection.height >= 0 && "Invalid intersection of scissor boxes!");*/
	SetScissorBox(intersection);
}
void ScissorBoxHandler::SetIntersectedScissorBox(const ScissorBox& scissorBox)
{
	SetIntersectedScissorBox(scissorBox.x, scissorBox.y, scissorBox.width, scissorBox.height);
}

ScissorBoxHandler::ScissorBox ScissorBoxHandler::GetScissorBox()
{
	return scissor_box_;
}

ScissorBoxHandler::ScissorBox ScissorBoxHandler::GetPrevScissorBox()
{
	return prev_scissor_box_;
}

void ScissorBoxHandler::EnableScissorTest()
{
	if (!scissor_test_enabled_)
	{
		glEnable(GL_SCISSOR_TEST);
		scissor_test_enabled_ = true;
	}
}

void ScissorBoxHandler::DisableScissorTest()
{
	if (scissor_test_enabled_)
	{
		glDisable(GL_SCISSOR_TEST);
		scissor_test_enabled_ = false;
	}
}

void ScissorBoxHandler::RestoreScissorBox()
{
	SetScissorBox(prev_scissor_box_);
}

bool ScissorBoxHandler::IsScissorTestEnabled() const
{
	return scissor_test_enabled_;
}

void ScissorBoxHandler::MemorizeScissorBox()
{
	//GLint prevScissorBox[4];
	//glGetIntegerv(GL_SCISSOR_BOX, prevScissorBox);
	//prev_scissor_box_ = { prevScissorBox[0], prevScissorBox[1], prevScissorBox[2], prevScissorBox[3] };
	prev_scissor_box_ = scissor_box_;
}

