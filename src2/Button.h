#pragma once
#include <vector>
#include <algorithm>
#include <memory>
#include <glm/glm.hpp>
#include "GameObject.h"
#include "Text.h"
#include "Capsule.h"
#include "TextRenderer.h"
#include "CircleRenderer.h"
#include "ColorRenderer.h"

// State of the button
enum ButtonState {
	Inactive,
	Normal,
	Hover,
	Pressed
};

// A button has a a text and by default a rectangle that wraps the text.
class Button {
public:
	// Default constructor
	Button() = default;
	// Default destructor
	~Button() = default;
	// Constructs a new Button instance.
	Button(glm::vec2 position, glm::vec2 size, std::string content, bool textOnCenter = true, glm::vec4 color = glm::vec4(Button::buttonColor, 1.0f), glm::vec3 textColor = Button::buttonTextColor,  ButtonState state = ButtonState::Inactive);

	// Getters and setters for the button.
	Text& GetText();
	void SetDirection(int direction);
	int GetDirection() const;
	void SetState(ButtonState state);
	ButtonState GetState() const;
	void SetPosition(glm::vec2 position);
	glm::vec2 GetPosition() const;
	void SetSize(glm::vec2 size);
	glm::vec2 GetSize() const;
	// Set to true if the text should be centered in the button.
	void SetTextOnCenter(bool textOnCenter);
	bool IsTextOnCenter() const;

	// Check if a position is inside the button.
	bool IsPositionInside(glm::vec2 position) const;
	// Draw the button.
	void Draw(std::shared_ptr<TextRenderer> textRenderer, std::shared_ptr<ColorRenderer> colorRenderer);

	// color of the button and the text
	static glm::vec3 buttonColor;
	static glm::vec3 buttonTextColor;
private:
	glm::vec2 position_, size_;
	std::unique_ptr<Text> text_;
	bool text_on_center_;
	glm::vec4 color_;
	ButtonState state_;
};