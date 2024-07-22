/*
 * Shooter.cpp
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

#include "Shooter.h"

Shooter::Shooter(glm::vec2 pos, glm::vec2 size, glm::vec2 rotationPivot,
                 glm::vec2 tailPivot, Texture2D sprite)
    : GameObject(pos, size, 0.0f, rotationPivot, glm::vec2(0.0f, 0.0f),
                 glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), sprite) {
  glm::vec2 rotationCenter(position + rotationPivot * size);
  glm::vec2 tail(position + tailPivot * size);
  carriedBubble = Bubble(glm::vec2(rotationCenter.x - kBubbleRadius,
                                   rotationCenter.y - kBubbleRadius),
                         kBubbleRadius, glm::vec2(0.0f, 0.0f),
                         glm::vec4(0.0f, 1.0f, 0.0f, 0.8f),
                         ResourceManager::GetInstance().GetTexture("bubble"));
  nextBubble = Bubble(glm::vec2(tail.x - kBaseUnit / 2, tail.y - kBaseUnit / 2),
                      kBaseUnit / 2, glm::vec2(0.0f, 0.0f),
                      glm::vec4(0.0f, 0.0f, 1.0f, 0.7f),
                      ResourceManager::GetInstance().GetTexture("bubble"));
  // Calculate the rotation pivot for the next bubble. This rotation pivot is
  // the point that the next bubble rotates around.
  glm::vec2 distance = rotationCenter - nextBubble.GetPosition();
  glm::vec2 rotationPivotForNextBubble(distance / nextBubble.GetSize());
  nextBubble.SetRotationPivot(rotationPivotForNextBubble);
  // Initialize the ray
  // ray = Ray(rotationCenter, GetShootingDirection(),
  //          glm::vec4(0.24314, 0.08627, 0.00392, 1.0f));
  ray = Ray(rotationCenter, GetShootingDirection(),
            glm::vec4(1.f, 1.f, 1.f, 1.0f));
}

Shooter::~Shooter() {}

void Shooter::RefreshCarriedBubbleColor(glm::vec3 color) {
  carriedBubble.SetColor(glm::vec4(color, carriedBubble.GetColor().a));
  //// Set the color for the ray
  // ray.SetColor(glm::vec4(glm::vec3(carriedBubble.GetColorWithoutAlpha()), 1.0f));
}

void Shooter::RefreshNextBubbleColor(glm::vec3 color) {
  nextBubble.SetColor(glm::vec4(color, nextBubble.GetColor().a));
}

std::unique_ptr<Bubble> Shooter::ShootBubble() {
  std::unique_ptr<Bubble> copiedBubble =
      std::make_unique<Bubble>(carriedBubble);
  copiedBubble->SetVelocity(this->GetShootingDirection() * 16.0f *
                            kVelocityUnit);
  // RGB
  glm::vec3 rgb(nextBubble.GetColor().r, nextBubble.GetColor().g,
                nextBubble.GetColor().b);
  carriedBubble.SetColor(glm::vec4(rgb, carriedBubble.GetColor().a));
  // Create a new color for the next bubble
  nextBubble.SetColor(GetNewBubbleColor());
  //// Set the color for the ray
  // ray.SetColor(glm::vec4(glm::vec3(carriedBubble.GetColorWithoutAlpha()), 1.0f));
  return copiedBubble;
}

std::unique_ptr<Bubble> Shooter::ShootBubble(glm::vec4 nextBubbleColor) {
  // Play the sound effect for shooting a bubble
  SoundEngine::GetInstance().PlaySound("bubble_pop", false);
  std::unique_ptr<Bubble> copiedBubble =
      std::make_unique<Bubble>(carriedBubble);
  copiedBubble->SetVelocity(this->GetShootingDirection() * 16.0f *
                            kVelocityUnit);
  // RGB
  glm::vec3 rgb(nextBubble.GetColor().r, nextBubble.GetColor().g,
                nextBubble.GetColor().b);
  carriedBubble.SetColor(glm::vec4(rgb, carriedBubble.GetColor().a));
  // Create a new color for the next bubble
  nextBubble.SetColor(nextBubbleColor);
  //// Set the color for the ray
  // ray.SetColor(glm::vec4(glm::vec3(carriedBubble.GetColorWithoutAlpha()), 1.0f));
  return copiedBubble;
}

void Shooter::GenerateBubble() {
  //// Otherwise, generate a new bubble. The bubble is always at the rotation
  /// center of the shooter. The radius of it is equal to the regular bubble
  /// radius provided by the ResourceManager.
  // glm::vec2 rotationCenter(position.x+rotationPivot.x*size.x,
  // position.y+rotationPivot.y*size.y); carriedBubble =
  // std::make_unique<Bubble>(glm::vec2(rotationCenter.x - kBubbleRadius,
  // rotationCenter.y - kBubbleRadius), kBubbleRadius, glm::vec2(0.0f, 0.0f),
  // glm::vec4(0.0f, 1.0f, 0.0f, 0.8f),
  // ResourceManager::GetInstance().GetTexture("bubble"));
}

void Shooter::SetRoll(float roll) {
  // Set the roll for the shooter
  GameObject::SetRoll(roll);
  // Set the roll for the next bubble
  nextBubble.SetRoll(roll);
  // Set the direction for the ray
  ray.SetDirection(GetShootingDirection());
}

void Shooter::SetPosition(glm::vec2 position) {
  // Set the position for the shooter
  GameObject::SetPosition(position);
  // Set the position for the carried bubble
  glm::vec2 rotationCenter(position + rotationPivot * size);
  // Get the position offset between the nextBubble and the carriedBubble
  glm::vec2 offset = nextBubble.GetPosition() - carriedBubble.GetPosition();
  carriedBubble.SetPosition(glm::vec2(rotationCenter.x - kBubbleRadius,
                                      rotationCenter.y - kBubbleRadius));
  // Set the position for the next bubble based on its offset from the carried
  // bubble
  nextBubble.SetPosition(carriedBubble.GetPosition() + offset);
  // Set the position for the ray
  ray.SetPosition(rotationCenter);
}

glm::vec2 Shooter::GetShootingDirection() {
  float radians = roll - glm::pi<float>() / 2;
  return glm::vec2(glm::cos(radians), glm::sin(radians));
}

glm::vec4 Shooter::GetNewBubbleColor() {
  // Get a random color from Color predefined in the resource manager
  static std::random_device rd;   // Initialize a random device
  static std::mt19937 gen(rd());  // Seed the generator
  static std::uniform_int_distribution<> dis(
      0,
      static_cast<int>(colorMap.size()) - 1);  // Distribution for enum values

  Color randomColor =
      static_cast<Color>(dis(gen));  // Get a random color enum value
  // Set the color
  return glm::vec4(colorMap[randomColor], nextBubble.GetColor().a);
}

glm::vec4 Shooter::GetNewBubbleColor(
    const std::unordered_map<int, std::unique_ptr<Bubble>>& statics) {
  // Create a color map for each color's probability
  std::unordered_map<Color, float> colorProbalibity;
  // Iterate thourgh the color map and initialize the probability for each color
  // to 0
  float total = 0.0f;
  for (auto color : colorMap) {
    colorProbalibity[color.first] = 0.5f;
    total += 0.5f;
  }
  // Iterate through the static bubbles and count the number of each color
  for (auto& bubble : statics) {
    Color color = bubble.second->GetColorEnum();
    if (areFloatsEqual(colorProbalibity[color], 0.5f)) {
      colorProbalibity[color] = 1.5f;
      total += 1.f;
    }
  }
  // Create a random float between 0 and total
  int random = generateRandom(0.f, total);
  // Iterate through the color map and find the color that the random float
  // falls into
  float sum = 0.0f;
  for (auto color : colorProbalibity) {
    sum += color.second;
    if (random <= sum) {
      return glm::vec4(colorMap[color.first], nextBubble.GetColor().a);
    }
  }
  return glm::vec4(colorMap[Color::White], nextBubble.GetColor().a);
}

const Bubble& Shooter::GetCarriedBubble() const { return carriedBubble; }

void Shooter::UpdateCarriedBubbleRadius(float radius) {
  // Update the radius for the carried bubble
  carriedBubble.SetRadius(kBubbleRadius);
}

void Shooter::Draw(std::shared_ptr<Renderer> renderer) {
  // Draw the shooter
  GameObject::Draw(renderer);
  // Draw the bubble that the shooter is carrying
  carriedBubble.Draw(renderer);
  // Draw the next bubble
  nextBubble.Draw(renderer);
}

Ray& Shooter::GetRay() { return ray; }
