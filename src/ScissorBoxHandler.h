#pragma once

#include <glad/glad.h>
#include <cassert>
#include <climits>
#include <utility>
#include "ResourceManager.h"

class ScissorBoxHandler
{
public:
	struct ScissorBox
	{
		GLint x;
		GLint y;
		GLsizei width;
		GLsizei height;

		ScissorBox() : x(0), y(0), width(INT_MAX), height(INT_MAX) {}
		ScissorBox(GLint x, GLint y, GLsizei width, GLsizei height) : x(x), y(y), width(width), height(height) {}

	};

	static ScissorBoxHandler& GetInstance() {
		static ScissorBoxHandler instance;
		return instance;
	}

	// Set the expected window size.
	void SetExpectedWindowSizePadding(SizePadding sizePadding);
	// Set the actual window size.
	void SetActualWindowSizePadding(SizePadding sizePadding);



	// Set scissor box, return the adjusted scissor box
	void SetScissorBox(GLint x, GLint y, GLsizei width, GLsizei height);
	void SetScissorBox(const ScissorBox& scissorBox);

	// Set scissor box that is the intersection of the current scissor box and the specified scissor box.
	void SetIntersectedScissorBox(GLint x, GLint y, GLsizei width, GLsizei height);
	void SetIntersectedScissorBox(const ScissorBox& scissorBox);
	
	int GetExpectedWindowWidth() const;
	int GetExpectedWindowHeight() const;
	int GetActualWindowWidth() const;
	int GetActualWindowHeight() const;
	SizePadding GetExpectedWindowSizePadding() const;
	SizePadding GetActualWindowSizePadding() const;

	// Get scissor box
	ScissorBox GetScissorBox();

	// Get previous scissor box
	ScissorBox GetPrevScissorBox();
    
	void EnableScissorTest();
	void DisableScissorTest();
	void RestoreScissorBox();

	bool IsScissorTestEnabled() const;

private:
	// Private Constructor and Destructor
	ScissorBoxHandler();
	~ScissorBoxHandler();
	// Non-copyable and non-movable
	ScissorBoxHandler(const ScissorBoxHandler&) = delete;
	ScissorBoxHandler& operator=(const ScissorBoxHandler&) = delete;
	ScissorBoxHandler(ScissorBoxHandler&&) = delete;
	ScissorBoxHandler& operator=(ScissorBoxHandler&&) = delete;

	// Memorize the current scissor box into prev_scissor_box_.
	void MemorizeScissorBox();

	ScissorBox scissor_box_;
	ScissorBox prev_scissor_box_;
	bool scissor_test_enabled_;

	// Expected window size and paddings.
	SizePadding expected_window_size_padding_;

	// Actual window size and padding.
	SizePadding actual_window_size_padding_;
};