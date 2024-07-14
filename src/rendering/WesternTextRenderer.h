#pragma once
#include "TextRenderer.h"

class WesternTextRenderer : public TextRenderer {
 public:
  WesternTextRenderer(const Shader& shader, unsigned int width,
                      unsigned int height, char32_t benchmarkChar = U'H')
      : TextRenderer(shader, width, height, benchmarkChar) {}

  // Get the height and width of the text
  std::pair<glm::vec3, bool> GetTextSize(
      std::u32string text, float scale, float lineWidth,
      float lineSpacingFactor = 1.2f, float additionalPadding = 10.0f) override;
  // renders a string of text using the precompiled list of characters, return
  // the bottom left corner of the rendered text and the spacing between lines
  std::pair<float, float> RenderText(std::u32string text, float x, float y,
                                     float scale, float lineWidth,
                                     float lineSpacingFactor = 1.2f,
                                     float additionalPadding = 10.0f,
                                     glm::vec3 color = glm::vec3(0.0f),
                                     float alpha = 1.0f) override;
  std::pair<float, float> RenderCenteredText(
      std::u32string text, float x, float y, float scale, float lineWidth,
      float lineSpacingFactor = 1.2f, float additionalPadding = 10.0f,
      glm::vec2 center = kWindowSize * 0.5f, glm::vec3 color = glm::vec3(0.0f),
      float alpha = 1.0f) override;

  std::pair<float, float> RenderRightAlignedText(
      std::u32string text, float x, float y, float scale, float lineWidth,
      float lineSpacingFactor, float additionalPadding,
      glm::vec3 color = glm::vec3(0.0f), float alpha = 1.0f) override;
};