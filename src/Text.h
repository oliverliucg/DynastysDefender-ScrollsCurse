#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "Capsule.h"
#include "ResourceManager.h"
#include "TextRenderer.h"

// This class represents a text object in the game.
class Text {
 public:
  // Default constructor
  Text() = default;
  // Constructs a new Text instance.
  Text(glm::vec2 pos, float lineWidth,
       glm::vec4 boxBounds = glm::vec4(0.f, 0.f, kWindowSize),
       glm::vec3 color = glm::vec3(0.f), float scale = 1.0f,
       float lineSpacingFactor = 1.2f,
       float additionalPadding = kBubbleRadius / 3.0f);
  // Copy constructor
  Text(const Text& other);
  // Destructor
  ~Text();
  // Assignment operator
  Text& operator=(const Text& other);
  // Getters and setters for the text.
  std::u32string GetParagraph(int index) const;
  void SetParagraph(int index, const std::u32string& text);
  void AddParagraph(const std::u32string& text);
  void RemoveParagraph(int index);
  glm::vec4 GetBoxBounds();
  void SetBoxBounds(glm::vec4 newBoxBounds);
  glm::vec2 GetPosition() const;
  void SetPosition(glm::vec2 pos);
  float GetLineWidth() const;
  void SetLineWidth(float width);
  glm::vec2 GetCenter() const;
  void SetCenter(glm::vec2 center);
  glm::vec3 GetScrollRelationShip();
  void SetScrollRelationShip(glm::vec3 relation);
  void SetColor(glm::vec3 color);
  glm::vec3 GetColor();
  void SetAlpha(float alpha);
  float GetAlpha();
  void SetScale(float scale);
  float GetScale();
  void SetTargetScale(const std::string& target, float scale);
  float GetTargetScale(const std::string& target) const;
  glm::vec2 GetTextSize(std::shared_ptr<TextRenderer> textRenderer) const;
  Capsule& GetScrollIcon();
  bool IsScrollIconInitialized();
  void Draw(std::shared_ptr<TextRenderer> textRenderer,
            bool textCenteringEnabled = false);

 private:
  std::vector<std::u32string> paragraphs;
  glm::vec2 position, center;
  glm::vec4 boxBounds;
  glm::vec3 color;
  float alpha;
  float lineWidth, scale, lineSpacingFactor, additionalPadding;
  // target scales for the text to be scaled to.
  std::unordered_map<std::string, float> targetScales;
  // Scroll icon. a capsule with two semicircles and a rectangle.
  std::unique_ptr<Capsule> scrollIcon;
  // The linear relationship between the center of the scroll icon and the
  // position of the text.
  glm::vec3 scrollRelation;
};
