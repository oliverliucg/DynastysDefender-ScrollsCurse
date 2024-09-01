/*
 * Bubble.cpp
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

#include "Bubble.h"

Bubble::Bubble() : GameObject() {}

Bubble::Bubble(glm::vec2 pos, float radius, glm::vec2 velocity, glm::vec4 color,
               Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), 0.0f,
                 glm::vec2(0.5f, 0.5f), velocity, color, sprite),
      radius(radius) {}

Bubble::Bubble(const Bubble& other) : GameObject(other) {
  radius = other.radius;
  state = other.state;
}

Bubble::Bubble(Bubble&& other) noexcept : GameObject(other) {
  radius = other.radius;
  state = other.state;
}

Bubble& Bubble::operator=(Bubble&& other) noexcept {
  GameObject::operator=(other);
  radius = other.radius;
  state = other.state;
  return *this;
}

Bubble& Bubble::operator=(const Bubble& other) {
  GameObject::operator=(other);
  radius = other.radius;
  state = other.state;
  return *this;
}

bool Bubble::Move(float deltaTime, glm::vec4 boundaries,
                  std::unordered_map<int, std::unique_ptr<Bubble> >& statics) {
  bool isPenetrating = false;
  position += velocity * deltaTime;
  if (position.x < boundaries.x) {
    float deltaX = position.x - boundaries.x;
    float deltaY = velocity.y / velocity.x * deltaX;
    position.x = boundaries.x;
    position.y -= deltaY;
    velocity.x *= -1.0f;
  } else if (position.x + size.x > boundaries.z) {
    float deltaX = position.x + size.x - boundaries.z;
    float deltaY = velocity.y / velocity.x * deltaX;
    position.x = boundaries.z - size.x;
    position.y -= deltaY;
    velocity.x *= -1.0f;
  }
  if (position.y < boundaries.y) {
    float deltaY = position.y - boundaries.y;
    float deltaX = velocity.x / velocity.y * deltaY;
    position.y = boundaries.y;
    position.x -= deltaX;
    isPenetrating = true;
  } else if (position.y + size.y > boundaries.w) {
    float deltaY = position.y + size.y - boundaries.w;
    float deltaX = velocity.x / velocity.y * deltaY;
    position.y = boundaries.w - size.y;
    position.x -= deltaX;
    velocity.y *= -1.0f;
  }

  // Check penetration with static bubbles
  for (auto& staticBubble : statics) {
    glm::vec2 staticCenter = staticBubble.second->GetCenter();
    float staticRadius = staticBubble.second->GetRadius();
    // If the distance between the centers of the two bubbles is less than the
    // sum of their radius, then the bubble is penetrating the static bubble.
    // Move back the bubble to the point of penetration
    float distance = glm::distance(GetCenter(), staticCenter);
    if (distance < radius + staticRadius) {
      // Get normalized vector of velocity of the bubble
      glm::vec2 normalizedVelocity = glm::normalize(velocity);
      // Get the opposite direction of the normalized velocity
      glm::vec2 oppositeDirection = -normalizedVelocity;
      // Get normalized vector of the line that connects the centers of the two
      // bubbles
      glm::vec2 normalizedLine = glm::normalize(staticCenter - GetCenter());
      // Get the absolute distance between the centers of the two bubbles
      float absoluteDistance = glm::distance(GetCenter(), staticCenter);
      // Move the bubble back to the point of penetration. The point of
      // penetration is the point that is on the surface of the static bubble
      // and is on the line that connects the centers of the two bubbles. That
      // means the distance between the moving bubble and the center of the
      // static bubble is equal to the radius of the static bubble. First we
      // calculate the angle between the opposite direction and the line that
      // connects the centers of the two bubbles.
      float cosGamma = glm::dot(oppositeDirection, normalizedLine);
      bool isAcute = cosGamma > 0.0f;
      cosGamma = glm::abs(cosGamma);
      // Use law of cosines to calculate the distance between the centers of the
      // moving bubble at the point of penetration and at the current point.
      std::pair<float, float> distanceToPenetrationPoints = lawOfCosinesB(
          absoluteDistance,
          staticBubble.second->GetRadius() + this->GetRadius(), cosGamma);

      // If gamma is acute, then choose the longer distance (the second).
      // Otherwise, choose the shorter distance (the first).
      float distanceToPenetrationPoint =
          isAcute ? distanceToPenetrationPoints.second
                  : distanceToPenetrationPoints.first;
      distanceToPenetrationPoint = glm::abs(distanceToPenetrationPoint);
      /*float distanceToPenetrationPoint = distanceToPenetrationPoints.second;*/
      // Calculate the time to move from the penetration point to the current
      // point.
      float timeToMove = distanceToPenetrationPoint / glm::length(velocity);

      // Move the bubble back to the penetration point
      position -= velocity * timeToMove;

      isPenetrating = true;
    }
  }
  if (isPenetrating) {
    // Set the velocity to 0.
    velocity = glm::vec2(0.0f, 0.0f);
  }
  this->SetPosition(position);
  return isPenetrating;
}

void Bubble::Move(float deltaTime) {
  this->SetPosition(position + velocity * deltaTime);
}

void Bubble::SetRadius(float radius) {
  glm::vec2 center = GetCenter();
  this->radius = radius;
  this->size = glm::vec2(radius * 2.0f, radius * 2.0f);
  this->position = center - glm::vec2(radius, radius);
}

float Bubble::GetRadius() const { return radius; }

glm::vec2 Bubble::GetCenter() const {
  return position + glm::vec2(radius, radius);
}

void Bubble::ApplyGravity(float deltaTime) {
  velocity += glm::vec2(0.0f, 9.8f * kVelocityUnit) * deltaTime;
}
