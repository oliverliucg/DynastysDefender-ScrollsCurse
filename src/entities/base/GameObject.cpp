/*
 * GameObject.cpp
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

#include "GameObject.h"

GameObject::GameObject()
    : roll(0.0f),
      scale(1.0f),
      position(glm::vec2(0.0f, 0.0f)),
      size(0.0f),
      rotationPivot(glm::vec2(0.5f, 0.5f)),
      velocity(glm::vec2(0.0f, 0.0f)),
      acceleration(glm::vec2(0.f, 0.f)),
      color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
      sprite(),
      uniqueID(ResourceManager::GetInstance().GetAvailableID()) {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, float roll,
                       glm::vec2 rotationPivot, glm::vec2 velocity,
                       glm::vec4 color, Texture2D sprite)
    : roll(roll),
      scale(1.0f),
      position(pos),
      size(size),
      rotationPivot(rotationPivot),
      velocity(velocity),
      acceleration(glm::vec2(0.f, 0.f)),
      color(color),
      sprite(sprite),
      uniqueID(ResourceManager::GetInstance().GetAvailableID()) {}

GameObject::GameObject(const GameObject& other) {
  roll = other.roll;
  scale = other.scale;
  position = other.position;
  size = other.size;
  rotationPivot = other.rotationPivot;
  velocity = other.velocity;
  acceleration = other.acceleration;
  color = other.color;
  sprite = other.sprite;
  uniqueID = ResourceManager::GetInstance().GetAvailableID();
}

GameObject& GameObject::operator=(const GameObject& other) {
  roll = other.roll;
  scale = other.scale;
  position = other.position;
  size = other.size;
  rotationPivot = other.rotationPivot;
  velocity = other.velocity;
  acceleration = other.acceleration;
  color = other.color;
  sprite = other.sprite;
  uniqueID = ResourceManager::GetInstance().GetAvailableID();
  return *this;
}

GameObject::GameObject(GameObject&& other) noexcept {
  roll = other.roll;
  scale = other.scale;
  position = other.position;
  size = other.size;
  rotationPivot = other.rotationPivot;
  velocity = other.velocity;
  acceleration = other.acceleration;
  color = other.color;
  sprite = other.sprite;
  uniqueID = other.uniqueID;
  other.uniqueID = -1;
}

GameObject& GameObject::operator=(GameObject&& other) noexcept {
  roll = other.roll;
  scale = other.scale;
  position = other.position;
  size = other.size;
  rotationPivot = other.rotationPivot;
  velocity = other.velocity;
  acceleration = other.acceleration;
  color = other.color;
  sprite = other.sprite;
  uniqueID = other.uniqueID;
  other.uniqueID = -1;
  return *this;
}

GameObject::~GameObject() { ResourceManager::GetInstance().ReturnID(uniqueID); }

void GameObject::Draw(std::shared_ptr<Renderer> renderer) {
  glm::vec2 sizeAfterScaling = size * scale;
  glm::vec2 positionAfterScaling =
      position + size * rotationPivot - sizeAfterScaling * rotationPivot;
  // Try to cast to ColorRenderer
  std::shared_ptr<ColorRenderer> colorRenderer =
      std::dynamic_pointer_cast<ColorRenderer>(renderer);
  if (colorRenderer) {
    colorRenderer->DrawColor(positionAfterScaling, sizeAfterScaling, roll,
                             rotationPivot, color);
    return;
  }

  // Try to cast to SpriteRenderer
  std::shared_ptr<SpriteRenderer> spriteRenderer =
      std::dynamic_pointer_cast<SpriteRenderer>(renderer);
  if (spriteRenderer) {
    spriteRenderer->DrawSprite(sprite, positionAfterScaling, sizeAfterScaling,
                               roll, rotationPivot, color);
    return;
  }

  assert(false &&
         "Only support color renderer and sprite renderer for the game object");
}

void GameObject::Draw(std::shared_ptr<SpriteDynamicRenderer> renderer,
                      glm::vec4 texCoords) {
  glm::vec2 sizeAfterScaling = size * scale;
  glm::vec2 positionAfterScaling =
      position + size * rotationPivot - sizeAfterScaling * rotationPivot;
  renderer->DrawSprite(sprite, positionAfterScaling, sizeAfterScaling, roll,
                       rotationPivot, color, texCoords);
}

// Getters and setters
glm::vec2 GameObject::GetPosition() const { return position; }
glm::vec2 GameObject::GetCenter() const { return position + size * 0.5f; }
float GameObject::GetScale() const { return scale; }
glm::vec2 GameObject::GetSize() const { return size; }
float GameObject::GetRoll() const { return roll; }
glm::vec2 GameObject::GetRotationPivot() const { return rotationPivot; }
glm::vec2 GameObject::GetRotationPivotCoord() const {
  return position + size * rotationPivot;
}
glm::vec2 GameObject::GetVelocity() const { return velocity; }
glm::vec2 GameObject::GetAcceleration() const { return acceleration; }
glm::vec4 GameObject::GetColor() const { return color; }
glm::vec3 GameObject::GetColorWithoutAlpha() const {
  return glm::vec3(color.r, color.g, color.b);
}
float GameObject::GetAlpha() const { return color.a; }
Color GameObject::GetColorEnum() const {
  return colorToEnum(glm::vec3(color.r, color.g, color.b));
}
int GameObject::GetID() const { return uniqueID; }

void GameObject::SetPosition(glm::vec2 pos) { position = pos; }
void GameObject::SetCenter(glm::vec2 center) {
  position = center - size * 0.5f;
}
void GameObject::SetScale(float scale) { this->scale = scale; }
void GameObject::SetSize(glm::vec2 size) { this->size = size; }
void GameObject::SetRoll(float roll) {
  this->roll = roll;
  const float PI = glm::pi<float>();
  if (this->roll < -PI) {
    this->roll += 2 * PI;
  } else if (this->roll > PI) {
    this->roll -= 2 * PI;
  }
}
void GameObject::SetRotationPivot(glm::vec2 rotationPivot) {
  this->rotationPivot = rotationPivot;
}
void GameObject::SetVelocity(glm::vec2 velocity) { this->velocity = velocity; }
void GameObject::SetAcceleration(glm::vec2 acceleration) {
  this->acceleration = acceleration;
}
void GameObject::SetColor(glm::vec4 color) { this->color = color; }
void GameObject::SetColorWithoutAlpha(glm::vec3 color) {
  this->color = glm::vec4(color, this->color.a);
}
void GameObject::SetAlpha(float alpha) { this->color.a = alpha; }
void GameObject::SetTexture(Texture2D sprite) { this->sprite = sprite; }

void GameObject::StopMoving() {
  velocity = glm::vec2(0.f, 0.f);
  acceleration = glm::vec2(0.f, 0.f);
}
