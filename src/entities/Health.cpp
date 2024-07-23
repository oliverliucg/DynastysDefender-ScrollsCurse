/*
 * Health.cpp
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

#include "Health.h"

Health::Health()
    : totalHealth(0),
      currentHealth(0),
      healthBarEdgeColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
      healthBarFillColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
      damageTextInitialPosition(glm::vec2(0.f)),
      damageTextTargetPosition(glm::vec2(0.f)),
      damageTextFadedPosition(glm::vec2(0.f)),
      damagePopOutToRight(true),
      damageTextScale(1.f) {}

Health::Health(int totalHealth)
    : totalHealth(totalHealth),
      currentHealth(totalHealth),
      healthBarEdgeColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
      healthBarFillColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
      damageTextInitialPosition(glm::vec2(0.f)),
      damageTextTargetPosition(glm::vec2(0.f)),
      damageTextFadedPosition(glm::vec2(0.f)),
      damagePopOutToRight(true),
      damageTextScale(1.f) {}

Health::Health(const Health& other)
    : totalHealth(other.totalHealth),
      currentHealth(other.currentHealth),
      healthBarEdgeColor(other.healthBarEdgeColor),
      healthBarFillColor(other.healthBarFillColor),
      damageTextInitialPosition(other.damageTextInitialPosition),
      damageTextTargetPosition(other.damageTextTargetPosition),
      damageTextFadedPosition(glm::vec2(0.f)),
      damagePopOutToRight(true),
      damageTextScale(1.f) {}

Health::Health(Health&& other) noexcept
    : totalHealth(other.totalHealth),
      currentHealth(other.currentHealth),
      healthBarEdgeColor(other.healthBarEdgeColor),
      healthBarFillColor(other.healthBarFillColor),
      damageTextInitialPosition(other.damageTextInitialPosition),
      damageTextTargetPosition(other.damageTextTargetPosition),
      damageTextFadedPosition(glm::vec2(0.f)),
      damagePopOutToRight(true),
      damageTextScale(1.f) {}

Health& Health::operator=(const Health& other) {
  totalHealth = other.totalHealth;
  currentHealth = other.currentHealth;
  healthBarEdgeColor = other.healthBarEdgeColor;
  healthBarFillColor = other.healthBarFillColor;
  damageTextInitialPosition = other.damageTextInitialPosition;
  damageTextTargetPosition = other.damageTextTargetPosition;
  damageTextFadedPosition = other.damageTextFadedPosition;
  damagePopOutToRight = other.damagePopOutToRight;
  damageTextScale = other.damageTextScale;
  return *this;
}

Health& Health::operator=(Health&& other) noexcept {
  totalHealth = other.totalHealth;
  currentHealth = other.currentHealth;
  healthBarEdgeColor = other.healthBarEdgeColor;
  healthBarFillColor = other.healthBarFillColor;
  damageTextInitialPosition = other.damageTextInitialPosition;
  damageTextTargetPosition = other.damageTextTargetPosition;
  damageTextFadedPosition = other.damageTextFadedPosition;
  damagePopOutToRight = other.damagePopOutToRight;
  damageTextScale = other.damageTextScale;
  return *this;
}

Health::~Health() {}

int Health::GetTotalHealth() const { return totalHealth; }

void Health::SetTotalHealth(int totalHealth) {
  this->totalHealth = totalHealth;
}

int Health::GetCurrentHealth() const { return currentHealth; }

void Health::SetCurrentHealth(int currentHealth) {
  // Set the current health
  this->currentHealth = currentHealth;
  assert(this->currentHealth <= totalHealth && this->currentHealth >= 0 &&
         "the health should be between 0 and the total health");
  // Adjust the size of the current health bar based on the proportion of the
  // current health to the total health
  float proportion = (float)currentHealth / (float)totalHealth;
  glm::vec4 color;
  // when proportion is greater then 2/3, the color is green
  if (proportion > 2.0f / 3.0f) {
    color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
  }
  // when proportion is greater then 1/3 and less then 2/3, the color is orange
  else if (proportion > 1.0f / 3.0f) {
    color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
  }
  // when proportion is less then 1/3, the color is red
  else {
    color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
  }
  healthBarFillColor = color;

  /*currentHealthBar.SetColor(color);*/

  //// Get the size of the current health bar based on the proportion of the
  //// current health to the total health
  // currentHealthBar.SetSize(glm::vec2(totalHealthBar->GetSize().x,
  //                                    totalHealthBar->GetSize().y *
  //                                    proportion));

  //// The position is always the same as the position of the total health bar
  // currentHealthBar.SetCenter(
  //     totalHealthBar->GetCenter() -
  //     glm::vec2(totalHealthBar.GetSize().y * (1.f - proportion) / 2.0f, 0));

  // currentHealthBar.SetRoll(glm::pi<float>() / 2.0f);
}

void Health::SetTotalHealthBar(glm::vec2 pos, glm::vec2 size, glm::vec4 color) {
  glm::vec2 capsuleSize = glm::vec2(size.y, size.x);
  glm::vec2 capsuleCenter = pos + size / 2.f;
  if (totalHealthBar == nullptr) {
    totalHealthBar =
        std::make_unique<Capsule>(capsuleCenter, capsuleSize, color);
  } else {
    totalHealthBar->SetSize(capsuleSize);
    totalHealthBar->SetCenter(capsuleCenter);
    totalHealthBar->SetColor(color);
  }
  totalHealthBar->SetRoll(glm::pi<float>() / 2.0f);

  // Set the initial position and target position and the faded position of the
  // damage text
  SetDamagePopOutDirection(this->damagePopOutToRight);

  // Set the scale of the damage text based on the size of the total health bar
  damageTextScale = 0.06f * size.y / kBaseUnit / kFontScale;
}

// void Health::SetCurrentHealthBar(glm::vec2 pos, glm::vec2 size,
//                                  glm::vec4 color) {
//   currentHealthBar.SetSize(glm::vec2(size.y, size.x));
//   currentHealthBar.SetCenter(pos + size / 2.0f);
//   currentHealthBar.SetColor(color);
//   currentHealthBar.SetRoll(glm::pi<float>() / 2.0f);
// }

Capsule& Health::GetTotalHealthBar() {
  assert(totalHealthBar != nullptr && "totalHealthBar should not be nullptr");
  return *totalHealthBar;
}

// Capsule& Health::GetCurrentHealthBar() { return currentHealthBar; }

void Health::IncreaseHealth(int mount) {
  if (mount == 0) return;
  // Create a new damage text
  damageTexts.push_back(Text(
      /*pos=*/this->totalHealthBar->GetCenter() + glm::vec2(0, 5 * kBaseUnit),
      /*lineWidth=*/10 * kBaseUnit));
  auto& damageText = damageTexts.back();
  damageText.SetPosition(this->totalHealthBar->GetCenter() +
                         glm::vec2(0, 5 * kBaseUnit));
  std::u32string damageStr = intToU32String(mount * 1200 / totalHealth);
  if (mount < 0) {
    damageText.SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
  } else {
    damageText.SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
    damageStr = U"+" + damageStr;
  }
  damageStr = U"{" + damageStr + U"}";
  damageText.AddParagraph(damageStr);
  damageText.SetScale(0.0005f / kFontScale);
  damageText.SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
  damageText.SetAlpha(1.0f);
  this->SetCurrentHealth(currentHealth + mount);
}
void Health::DecreaseHealth(int mount) { this->IncreaseHealth(-mount); }

void Health::SetDamagePopOutDirection(bool popOutToRight) {
  damagePopOutToRight = popOutToRight;
  if (!damagePopOutToRight) {
    damageTextInitialPosition =
        totalHealthBar->GetCenter() + glm::vec2(-kBaseUnit, 5.5f * kBaseUnit);
    damageTextTargetPosition =
        rotateVector(totalHealthBar->GetBottomSemiCircleCenter(),
                     totalHealthBar->GetRoll(), totalHealthBar->GetCenter()) +
        glm::vec2(-kBaseUnit, 2.7f * kBaseUnit);
  } else {
    damageTextInitialPosition =
        totalHealthBar->GetCenter() + glm::vec2(0, 5.5f * kBaseUnit);
    damageTextTargetPosition =
        rotateVector(totalHealthBar->GetTopSemiCircleCenter(),
                     totalHealthBar->GetRoll(), totalHealthBar->GetCenter()) +
        glm::vec2(0.f, 2.7f * kBaseUnit);
  }
  damageTextFadedPosition =
      damageTextTargetPosition - glm::vec2(0.0f, 5 * kBaseUnit);
}

void Health::UpdateDamageTexts(float dt) {
  // std::vector<Text> existingDamageTexts;
  std::vector<int> toRemove;
  for (size_t i = 0; i < damageTexts.size(); ++i) {
    auto& damageText = damageTexts[i];
    assert(damageText.GetAlpha() > 0.f &&
           "the alpha of the damage text should be greater than 0.");
    /*existingDamageTexts.emplace_back(damageText);*/
    bool isFadingOut = damageText.GetPosition().x == damageTextTargetPosition.x;
    glm::vec2 targetPosition =
        isFadingOut ? damageTextFadedPosition : damageTextTargetPosition;
    glm::vec2 initialPosition =
        isFadingOut ? damageTextTargetPosition : damageTextInitialPosition;
    // Get distance from the initial position to the target position
    float totalDistance = glm::distance(targetPosition, initialPosition);
    // Get the vector from the initial position to the target position
    glm::vec2 direction = glm::normalize(targetPosition - initialPosition);
    // Get the vector from the current position to the target position
    glm::vec2 currentDirection =
        glm::normalize(targetPosition - damageText.GetPosition());
    // Get the new position of the damage text
    float speed = isFadingOut ? 5.0f * kBaseUnit : 30.0f * kBaseUnit;
    glm::vec2 newPosition = damageText.GetPosition() + speed * direction * dt;
    // Get distance from the initial position to the new position
    float newDistance = glm::distance(initialPosition, newPosition);
    // Get the vector from the new position to the target position
    glm::vec2 newDirection = glm::normalize(targetPosition - newPosition);
    // Get the inner product of the new direction and the direction
    float innerProduct = glm::dot(direction, newDirection);
    if (innerProduct > 0) {
      // If the inner product is greater than 0, the damage text is still moving
      // the target position
      damageText.SetPosition(newPosition);
      if (!isFadingOut) {
        // Increase the scale of the damage text based on the proportion of the
        // distance from the target position to the initial position to the
        // distance from the current position to the initial position
        float scale = damageTextScale * newDistance / totalDistance;
        damageText.SetScale(scale);
      } else {
        // Decrease the alpha of the damage text based on the proportion of the
        // distance from the target position to the initial position to the
        // distance from the current position to the initial position
        float alpha = (totalDistance - newDistance) / totalDistance;
        damageText.SetAlpha(alpha);
      }
    } else {
      if (!isFadingOut) {
        // If the inner product is less than 0, set the position of the damage
        // text to the target position
        damageText.SetPosition(targetPosition);
        damageText.SetScale(damageTextScale);
      } else {
        // It is totally faded out. We can remove it from the list.
        toRemove.emplace_back(i);
      }
    }
  }
  // Remove the totally faded out damage texts
  int offset = 0;
  for (auto& index : toRemove) {
    damageTexts.erase(damageTexts.begin() + index - offset);
    ++offset;
  }
  /*std::swap(this->damageTexts, existingDamageTexts);*/
}

void Health::DrawHealthBar(std::shared_ptr<ColorRenderer> colorRenderer,
                           std::shared_ptr<CircleRenderer> circleRenderer) {
  totalHealthBar->SetColor(healthBarEdgeColor);
  totalHealthBar->DrawOutline(colorRenderer, circleRenderer);
  // if (currentHealthBar.GetSize().y - currentHealthBar.GetSize().x >= 0.f) {
  //   currentHealthBar.Draw(colorRenderer, circleRenderer);
  // }

  if (currentHealth == totalHealth) {
    // Draw the total health bar
    totalHealthBar->SetColor(healthBarFillColor);
    totalHealthBar->Draw(colorRenderer, circleRenderer);
  } else if (currentHealth > 0) {
    totalHealthBar->SetColor(healthBarFillColor);
    float proportion = (float)currentHealth / (float)totalHealth;
    // apply scissors to the health bar
    ScissorBoxHandler& handler = ScissorBoxHandler::GetInstance();
    bool scissorTestEnabled = handler.IsScissorTestEnabled();
    handler.EnableScissorTest();
    glm::vec4 boundingBox = GetCurrentHealthBarBoundingBox();
    GLsizei scissorBoxWidth = std::ceil(boundingBox.z - boundingBox.x);
    GLsizei scissorBoxHeight = std::ceil(boundingBox.w - boundingBox.y);
    ScissorBoxHandler::ScissorBox barScissorBox(
        boundingBox.x, kWindowSize.y - boundingBox.w, scissorBoxWidth,
        scissorBoxHeight);
    handler.SetScissorBox(barScissorBox);
    totalHealthBar->Draw(colorRenderer, circleRenderer);

    // Restore the scissor box
    handler.RestoreScissorBox();
    if (!scissorTestEnabled) {
      handler.DisableScissorTest();
    }
  }
}

void Health::DrawDamageTexts(std::shared_ptr<TextRenderer> textRenderer) {
  for (auto& damageText : damageTexts) {
    damageText.Draw(textRenderer);
  }
}

glm::vec4 Health::GetCurrentHealthBarBoundingBox() {
  glm::vec2 center = totalHealthBar->GetCenter();
  glm::vec2 size =
      glm::vec2(totalHealthBar->GetSize().y, totalHealthBar->GetSize().x);
  glm::vec4 bounds =
      glm::vec4(center.x - size.x / 2.0f, center.y - size.y / 2.0f,
                center.x + size.x / 2.0f, center.y + size.y / 2.0f);
  float proportion = (float)currentHealth / (float)totalHealth;
  bounds.z = bounds.x + size.x * proportion;
  return bounds;
}
