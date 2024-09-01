/*
 * PowerUp.cpp
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

#include "PowerUp.h"

PowerUp::PowerUp(glm::vec2 pos, float radius, Texture2D stonePlateSprite,
                 Texture2D spindleSprite, Texture2D daggerSprite,
                 int numOfDaggers)
    : Bubble(pos, radius, glm::vec2(0.f), glm::vec4(1.f), stonePlateSprite),
      spindle(pos + glm::vec2(radius * 0.71f, radius * 0.71f),
              glm::vec2(radius * 0.58f, radius * 0.58f), 0.f,
              glm::vec2(0.5f, 0.5f), glm::vec2(0.f), glm::vec4(1.f),
              spindleSprite),
      dagger(pos + glm::vec2(radius * 0.923f, 0.f),
             glm::vec2(radius * 0.154f, radius * 0.77f), 0.f,
             glm::vec2(0.5f, 1.2987013f), glm::vec2(0.f), glm::vec4(1.f),
             daggerSprite),
      numOfDaggers(numOfDaggers) {}

PowerUp::~PowerUp() {}

void PowerUp::SetNumOfDaggers(int numOfDaggers) {
  this->numOfDaggers = numOfDaggers;
}

int PowerUp::GetNumOfDaggers() const { return numOfDaggers; }

void PowerUp::SetDaggerRotationSpeed(float speed) {
  this->daggerRotationSpeed = speed;
}

float PowerUp::GetDaggerRotationSpeed() const { return daggerRotationSpeed; }

void PowerUp::SetPowerUpState(PowerUpState state) { powerUpState = state; }

PowerUpState PowerUp::GetPowerUpState() const { return powerUpState; }

void PowerUp::SetPosition(glm::vec2 pos) {
  Bubble::SetPosition(pos);
  spindle.SetPosition(pos + glm::vec2(radius * 0.71f, radius * 0.71f));
  dagger.SetPosition(pos + glm::vec2(radius * 0.923f, 0.f));
}

void PowerUp::SetRadius(float radius) {
  Bubble::SetRadius(radius);
  spindle.SetSize(glm::vec2(radius * 0.58f, radius * 0.58f));
  dagger.SetSize(glm::vec2(radius * 0.154f, radius * 0.77f));
}

bool PowerUp::Move(float deltaTime, glm::vec4 boundaries,
                   std::unordered_map<int, std::unique_ptr<Bubble> >& statics) {
  this->bubblesToBeDestroyed.clear();
  position += velocity * deltaTime;
  if (position.x < boundaries.x) {
    float deltaX = position.x - boundaries.x;
    float deltaY = velocity.y / velocity.x * deltaX;
    position.x = boundaries.x;
    position.y -= deltaY;
    velocity.x *= -1.0f;
    // Set the rotation direction of the dagger to the opposite of the current.
    daggerRotationSpeed *= -1.0f;
  } else if (position.x + size.x > boundaries.z) {
    float deltaX = position.x + size.x - boundaries.z;
    float deltaY = velocity.y / velocity.x * deltaX;
    position.x = boundaries.z - size.x;
    position.y -= deltaY;
    velocity.x *= -1.0f;
    // Set the rotation direction of the dagger to the opposite of the current.
    daggerRotationSpeed *= -1.0f;
  }
  if (position.y < boundaries.y) {
    float deltaY = position.y - boundaries.y;
    float deltaX = velocity.x / velocity.y * deltaY;
    position.y = boundaries.y;
    position.x -= deltaX;
    velocity.y *= -1.0f;
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
    // If the distance between the static bubble and the powerup is less than
    // the sum of their radius, then the bubble is penetrating the static
    // bubble.
    float distance = glm::distance(GetCenter(), staticCenter);
    if (distance < radius + staticRadius) {
      this->bubblesToBeDestroyed.emplace_back(staticBubble.first);
    }
  }
  this->SetPosition(position);
  // Sort the bubbles to be destroyed based on their distance to the powerup
  // in ascending order
  std::sort(this->bubblesToBeDestroyed.begin(),
            this->bubblesToBeDestroyed.end(), [this, &statics](int a, int b) {
              return glm::distance(statics[a]->GetCenter(), GetCenter()) <
                     glm::distance(statics[b]->GetCenter(), GetCenter());
            });
  if (this->bubblesToBeDestroyed.size() > numOfDaggers) {
    assert(numOfDaggers > 0 &&
           "The number of daggers should be greater than 0.");
    // Remove the last bubbles from the list to match the number of daggers
    while (this->bubblesToBeDestroyed.size() > numOfDaggers) {
      this->bubblesToBeDestroyed.pop_back();
    }
  }
  numOfDaggers -= this->bubblesToBeDestroyed.size();
  return !this->bubblesToBeDestroyed.empty();
}

const std::vector<int>& PowerUp::GetBubblesToBeDestroyed() const {
  return bubblesToBeDestroyed;
}

void PowerUp::Update(float dt) {
  // this->SetRoll(this->GetRoll() + stonePlateRotationSpeed * dt);
  // spindle.SetRoll(spindle.GetRoll() + daggerRotationSpeed * dt);
  if (powerUpState == PowerUpState::kActive) {
    dagger.SetRoll(dagger.GetRoll() + daggerRotationSpeed * dt);
  }
}

void PowerUp::InsertDagger() {
  switch (powerUpState) {
    case PowerUpState::kInactive:
      powerUpState = PowerUpState::kReady;
      break;
    case PowerUpState::kReady:
      powerUpState = PowerUpState::kActive;
      ++numOfDaggers;
      break;
    case PowerUpState::kActive:
      ++numOfDaggers;
      break;
    case PowerUpState::Undefined:
      break;
    default:
      break;
  }
}

void PowerUp::Reset() {
  this->SetRoll(0.f);
  spindle.SetRoll(0.f);
  dagger.SetRoll(0.f);
  this->numOfDaggers = 0;
  powerUpState = PowerUpState::kInactive;
}

void PowerUp::Draw(std::shared_ptr<Renderer> renderer) {
  if (powerUpState == PowerUpState::kActive) {
    Bubble::Draw(renderer);
    if (numOfDaggers > 0) {
      // Draggers are placed in a circle around the center of the stone plate.
      // The angle between each dagger is 360 / numberOfDaggers.
      float angle = 2.f * glm::pi<float>() / numOfDaggers;
      float initialRoll = dagger.GetRoll();
      for (int i = 0; i < numOfDaggers; i++) {
        dagger.SetRoll(initialRoll + i * angle);
        dagger.Draw(renderer);
      }
      // Restore the roll of the dagger
      dagger.SetRoll(initialRoll);
    }
    spindle.Draw(renderer);
  }
}
