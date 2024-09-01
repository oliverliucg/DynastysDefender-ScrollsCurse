/*
 * Text.cpp
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

#include "Text.h"

bool TypingEffect::Update(float dt) {
  bool notCompleted = currentTextLength < fullText.size();
  timer += dt;
  typingTimer += dt;
  cursorTimer += dt;
  float actualSpeed = speed * speedAdjustmentFactor;
  size_t numOfKeysTyped = 0;
  bool lastCharIsSpace = false;
  if (notCompleted) {
    size_t newAddedTextLength = static_cast<size_t>(timer * actualSpeed);
    if (newAddedTextLength > 0) {
      timer -= newAddedTextLength / actualSpeed;
      currentTextLength += newAddedTextLength;
      if (currentTextLength > fullText.size()) {
        currentTextLength = fullText.size();
      }
      // When new text is added, reset the cursor timer.
      cursorTimer = 0.f;
      // When new text is added, reset the speed adjustment factor.
      speedAdjustmentFactor = generateGaussianRandom(0.5f, 2.f, 1.2f);
      // Check if the last character is a space.
      if (fullText[currentTextLength - 1] == U' ') {
        lastCharIsSpace = true;
      }
      numOfKeysTyped = newAddedTextLength;
      typingTimer = 0.f;
    } else {
      // Still have typing sound when no new text is added if the language
      // typing speed is slower than English.
      if (speed < DEFAULT_ENGLISH_TYPING_SPEED) {
        float typingSpeed =
            DEFAULT_ENGLISH_TYPING_SPEED * speedAdjustmentFactor;
        numOfKeysTyped = static_cast<size_t>(typingTimer * typingSpeed);
        typingTimer -= numOfKeysTyped / typingSpeed;
      }
    }
    // Play typing sound.
    if (numOfKeysTyped > 0) {
      SoundEngine& soundEngine = SoundEngine::GetInstance();
      if (lastCharIsSpace) {
        soundEngine.PlaySound("key_space", false);
      } else {
        // Randomly choose between key_s and keys_s_j.
        if (randomBool(0.75f)) {
          soundEngine.PlaySound("key_s", false);
        } else {
          soundEngine.PlaySound("keys_s_j", false);
        }
      }
    }

  } else {
    timer = 0.f;
  }
  if (cursorEnabled) {
    while (cursorTimer >= cursorInterval) {
      cursorTimer -= cursorInterval;
      cursorVisible = !cursorVisible;
    }
  } else {
    cursorVisible = false;
    cursorTimer = 0.f;
  }
  return notCompleted;
}

std::u32string TypingEffect::GetVisibleText() const {
  std::u32string res = fullText.substr(0, currentTextLength);
  if (cursorEnabled && cursorVisible) {
    res += U'|';
  }
  return res;
}

Text::Text(glm::vec2 pos, float lineWidth, glm::vec4 boxBounds, glm::vec3 color,
           float scale, float lineSpacingFactor, float additionalPadding)
    : position(pos),
      center(pos + glm::vec2(0.f, lineWidth * 0.5f)),
      boxBounds(boxBounds),
      color(color),
      alpha(1.0f),
      lineWidth(lineWidth),
      scale(scale),
      lineSpacingFactor(lineSpacingFactor),
      additionalPadding(additionalPadding) {}

Text::Text(const Text& other) {
  paragraphs = other.paragraphs;
  position = other.position;
  center = other.center;
  boxBounds = other.boxBounds;
  color = other.color;
  alpha = other.alpha;
  lineWidth = other.lineWidth;
  scale = other.scale;
  lineSpacingFactor = other.lineSpacingFactor;
  additionalPadding = other.additionalPadding;
  scrollRelation = other.scrollRelation;
  if (other.scrollIcon != nullptr) {
    scrollIcon = std::make_unique<Capsule>(*other.scrollIcon);
  }
  targetScales = other.targetScales;
}

Text& Text::operator=(const Text& other) {
  if (this == &other) {
    return *this;
  }
  paragraphs = other.paragraphs;
  position = other.position;
  center = other.center;
  boxBounds = other.boxBounds;
  color = other.color;
  alpha = other.alpha;
  lineWidth = other.lineWidth;
  scale = other.scale;
  lineSpacingFactor = other.lineSpacingFactor;
  additionalPadding = other.additionalPadding;
  scrollRelation = other.scrollRelation;
  if (other.scrollIcon != nullptr) {
    scrollIcon = std::make_unique<Capsule>(*other.scrollIcon);
  }
  targetScales = other.targetScales;
  return *this;
}

Text::~Text() {
  for (const auto& paragraph : paragraphs) {
    TextRenderer::UnLoadIfNotUsed(paragraph);
  }
}

std::u32string Text::GetParagraph(int index) const { return paragraphs[index]; }

void Text::SetParagraph(int index, const std::u32string& text) {
  assert(index < paragraphs.size() && index >= 0 && "Index out of bounds");
  TextRenderer::Load(text);
  TextRenderer::UnLoadIfNotUsed(paragraphs[index]);
  paragraphs[index] = text;
  /*TextRenderer::Load(paragraphs[index]);*/
}

void Text::AddParagraph(const std::u32string& text) {
  paragraphs.emplace_back(text);
  TextRenderer::Load(text);
}

void Text::RemoveParagraph(int index) {
  TextRenderer::UnLoadIfNotUsed(paragraphs[index]);
  paragraphs.erase(paragraphs.begin() + index);
}

void Text::RemoveAllParagraphs(bool unloadUnusedCharacters) {
  if (unloadUnusedCharacters) {
    for (const auto& paragraph : paragraphs) {
      TextRenderer::UnLoadIfNotUsed(paragraph);
    }
  }

  paragraphs.clear();
}

glm::vec4 Text::GetBoxBounds() { return boxBounds; }

void Text::SetBoxBounds(glm::vec4 newBoxBounds) { boxBounds = newBoxBounds; }

glm::vec2 Text::GetPosition() const { return position; }

void Text::SetPosition(glm::vec2 pos) { position = pos; }

float Text::GetLineWidth() const { return lineWidth; }

void Text::SetLineWidth(float lineWidth) { this->lineWidth = lineWidth; }

glm::vec2 Text::GetCenter() const { return center; }

void Text::SetCenter(glm::vec2 center) { this->center = center; }

Capsule& Text::GetScrollIcon() {
  assert(scrollIcon != nullptr && "Scroll icon not initialized");
  return *scrollIcon;
}

bool Text::IsScrollIconInitialized() { return scrollIcon != nullptr; }

glm::vec3 Text::GetScrollRelationShip() { return scrollRelation; }

void Text::SetScrollRelationShip(glm::vec3 relation) {
  scrollRelation = relation;
}

void Text::SetColor(glm::vec3 color) { this->color = color; }

glm::vec3 Text::GetColor() { return color; }

void Text::SetAlpha(float alpha) { this->alpha = alpha; }

float Text::GetAlpha() { return alpha; }

void Text::SetScale(float scale) { this->scale = scale; }

float Text::GetScale() { return scale; }

void Text::SetTargetScale(const std::string& target, float scale) {
  targetScales[target] = scale;
}

float Text::GetTargetScale(const std::string& target) const {
  assert(targetScales.count(target) > 0 &&
         "Target scale for the text not found");
  return targetScales.at(target);
}

glm::vec2 Text::GetTextSize(std::shared_ptr<TextRenderer> textRenderer) const {
  glm::vec2 finalSize(0.f);
  for (int i = 0; i < paragraphs.size(); ++i) {
    const auto& [paragraphSize, hasDescendersInLastLine] =
        textRenderer->GetTextSize(paragraphs[i], scale, lineWidth,
                                  lineSpacingFactor, additionalPadding);
    finalSize.x = std::max(finalSize.x, paragraphSize.x);
    finalSize.y += paragraphSize.y;

    if (i + 1 < paragraphs.size()) {
      finalSize.y += 1.5f * paragraphSize.z;
    } else if (i + 1 == paragraphs.size() && hasDescendersInLastLine) {
      finalSize.y +=
          (TextRenderer::characterMap.at(U'g').at(CharStyle::REGULAR).Size.y -
           TextRenderer::characterMap.at(U'g')
               .at(CharStyle::REGULAR)
               .Bearing.y) *
          scale;
    }
  }
  return finalSize;
}

void Text::Draw(std::shared_ptr<TextRenderer> textRenderer,
                bool textCenteringEnabled, bool textRightAligned) {
  float x = position.x;
  float y = position.y;
  if (!textCenteringEnabled) {
    if (!textRightAligned) {
      for (int i = 0; i < paragraphs.size(); ++i) {
        auto [textBottomLeft, spacing] = textRenderer->RenderText(
            paragraphs[i], x, y, scale, lineWidth, lineSpacingFactor,
            additionalPadding, color, alpha);
        y = textBottomLeft + 1.5f * spacing;
      }
    } else {
      for (int i = 0; i < paragraphs.size(); ++i) {
        auto [textBottomLeft, spacing] = textRenderer->RenderRightAlignedText(
            paragraphs[i], x, y, scale, lineWidth, lineSpacingFactor,
            additionalPadding, color, alpha);
        y = textBottomLeft + 1.5f * spacing;
      }
    }
  } else {
    for (int i = 0; i < paragraphs.size(); ++i) {
      auto [textBottomLeft, spacing] = textRenderer->RenderCenteredText(
          paragraphs[i], x, y, scale, lineWidth, lineSpacingFactor,
          additionalPadding, center, color, alpha);
      y = textBottomLeft + 1.5f * spacing;
    }
  }
}

bool Text::UpdateTypingEffect(float dt) {
  if (!IsTypingEffectEnabled()) return false;
  size_t cursorPosition = 0;
  bool notCompleted = false;
  for (size_t i = 0; i < typingEffects.size(); ++i) {
    cursorPosition = i;
    auto& typingEffect = typingEffects[i];
    notCompleted = typingEffect.Update(dt);
    if (notCompleted) {
      break;
    }
    // if (i == typingEffects.size() - 1) {
    //   // All typing effects are completed. No need to update anymore.
    //   notCompleted = false;
    // }
  }
  paragraphs[cursorPosition] = typingEffects[cursorPosition].GetVisibleText();
  return notCompleted;
}

bool Text::IsTypingEffectEnabled() { return !typingEffects.empty(); }

void Text::EnableTypingEffect(float speed) {
  for (int i = 0; i < paragraphs.size(); ++i) {
    typingEffects.emplace_back(paragraphs[i], 0, speed);
    paragraphs[i].clear();
  }
}

void Text::DisableTypingEffect() {
  paragraphs.clear();
  for (auto& typingEffect : typingEffects) {
    paragraphs.emplace_back(typingEffect.fullText);
  }
  typingEffects.clear();
}

void Text::SetLineSpacingFactor(float factor) { lineSpacingFactor = factor; }
void Text::SetAdditionalPadding(float padding) { additionalPadding = padding; }

bool Text::fadeOut(float dt, glm::vec2 direction, float movingSpeed,
                   float fadingSpeed) {
  assert(this->GetAlpha() > 0.f && "Text is already invisible");
  // Move the text towards the target direction.
  glm::vec2 newPosition = position + direction * movingSpeed * dt;
  // Fade out the text.
  float alpha = this->GetAlpha() - fadingSpeed * dt;
  if (alpha < 0.f) {
    alpha = 0.f;
  }
  this->SetPosition(newPosition);
  this->SetAlpha(alpha);
  if (alpha == 0.f) {
    return true;
  }
  return false;
}
