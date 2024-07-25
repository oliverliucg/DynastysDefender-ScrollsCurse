#pragma once
#include <algorithm>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Capsule.h"
#include "CircleRenderer.h"
#include "ColorRenderer.h"
#include "GameObject.h"
#include "Text.h"
#include "TextRenderer.h"

// State of the button
enum class ButtonState { kInactive, kNormal, kHovered, kPressed };

// A button has a a text and by default a rectangle that wraps the text.
class Button {
 public:
  // Default constructor
  Button() = default;
  // Default destructor
  ~Button() = default;
  // Constructs a new Button instance.
  Button(glm::vec2 position, glm::vec2 size, std::u32string content,
         bool textOnCenter = true,
         glm::vec4 color = glm::vec4(Button::buttonColor, 1.0f),
         glm::vec3 textColor = Button::buttonTextColor,
         ButtonState state = ButtonState::kInactive);

  // Getters and setters for the button.
  Text& GetText();
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
  void Draw(std::shared_ptr<TextRenderer> textRenderer,
            std::shared_ptr<ColorRenderer> colorRenderer);

  // color of the button and the text
  static glm::vec3 buttonColor;
  static glm::vec3 buttonTextColor;

 private:
  glm::vec2 position_{0.f, 0.f}, size_{1.f, 1.f};
  std::unique_ptr<Text> text_;
  bool text_on_center_{true};
  glm::vec4 color_{1.f, 1.f, 1.f, 1.f};
  ButtonState state_{ButtonState::kInactive};
};