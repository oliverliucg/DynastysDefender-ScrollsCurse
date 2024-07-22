/*
 * Button.cpp
 * Copyright (C) 2024 Oliver Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "Button.h"

Button::Button(glm::vec2 position, glm::vec2 size, std::u32string content,
               bool textOnCenter, glm::vec4 color, glm::vec3 textColor,
               ButtonState state)
    : position_(position),
      size_(size),
      text_on_center_(textOnCenter),
      text_(std::make_unique<Text>(
          position + glm::vec2(size.x * 0.035f, size.y * 0.22f),
          size.x * 0.9f)),
      color_(color),
      state_(state) {
  text_->AddParagraph(content);
  text_->SetColor(textColor);
  text_->SetScale(0.81f * size.y / (kWindowSize.y * kFontScale));
  text_->SetCenter(position + size / 2.0f);
  this->text_on_center_ = textOnCenter;
}

Text& Button::GetText() { return *text_; }

void Button::SetState(ButtonState state) {
  this->state_ = state;
  switch (state) {
    case ButtonState::kNormal:
    case ButtonState::kPressed:
      color_ = glm::vec4(Button::buttonColor, 1.0f);
      break;
    case ButtonState::kHovered:
      // Make the color of the button deeper.
      color_ = glm::vec4(Button::buttonColor * 0.8f, 1.0f);
      break;
    default:
      color_ = glm::vec4(Button::buttonColor, 0.0f);
      break;
  }
}

ButtonState Button::GetState() const { return state_; }

void Button::SetPosition(glm::vec2 position) {
  position_ = position;
  text_->SetPosition(position + glm::vec2(size_.x * 0.05f, size_.y * 0.21f));
  text_->SetCenter(position_ + size_ / 2.0f);
}

glm::vec2 Button::GetPosition() const { return position_; }

void Button::SetSize(glm::vec2 size) {
  // Get the diff between the position of text and the position of the button.
  glm::vec2 diff = text_->GetPosition() - GetPosition();
  float scaleX = size.x / size_.x;
  float scaleY = size.y / size_.y;
  float scaleText = std::min(scaleX, scaleY);
  text_->SetScale(text_->GetScale() * scaleText);
  // Set the position of the text.
  text_->SetPosition(GetPosition() + diff * glm::vec2(scaleX, scaleY));
  // Set the new size.
  size_ = size;
  // Set the center of the text.
  text_->SetCenter(position_ + size_ / 2.0f);
  text_->SetLineWidth(size_.x * 0.9f);
}

glm::vec2 Button::GetSize() const { return size_; }

void Button::SetTextOnCenter(bool textOnCenter) {
  text_on_center_ = textOnCenter;
}

bool Button::IsTextOnCenter() const { return text_on_center_; }

bool Button::IsPositionInside(glm::vec2 position) const {
  return position.x >= position_.x && position.x <= position_.x + size_.x &&
         position.y >= position_.y && position.y <= position_.y + size_.y;
}

void Button::Draw(std::shared_ptr<TextRenderer> textRenderer,
                  std::shared_ptr<ColorRenderer> colorRenderer) {
  colorRenderer->DrawColor(position_, size_, 0.0f, glm::vec2(0.5f, 0.5f),
                           color_);
  text_->Draw(textRenderer, text_on_center_);
}

glm::vec3 Button::buttonColor = glm::vec3(0.80392f, 0.62745f, 0.55294f);
glm::vec3 Button::buttonTextColor = glm::vec3(1.0f, 1.0f, 1.0f);