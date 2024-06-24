#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "Capsule.h"
#include "ResourceManager.h"
#include "TextRenderer.h"

constexpr float DEFAULT_CURSOR_INTERVAL = 0.5f;
constexpr bool DEFAULT_CURSOR_VISIBILITY = true;
constexpr float DEFAULT_ENGLISH_TYPING_SPEED = 10.f;

inline std::unordered_map<Language, float> typingSpeeds = {
    {Language::GERMAN, 1.0f * DEFAULT_ENGLISH_TYPING_SPEED},
    {Language::ENGLISH, 1.0f * DEFAULT_ENGLISH_TYPING_SPEED},
    {Language::SPANISH, 1.0f * DEFAULT_ENGLISH_TYPING_SPEED},
    {Language::ITALIAN, 1.0f * DEFAULT_ENGLISH_TYPING_SPEED},
    {Language::FRENCH, 1.0f * DEFAULT_ENGLISH_TYPING_SPEED},
    {Language::JAPANESE,
     0.6f *
         DEFAULT_ENGLISH_TYPING_SPEED},  // Japanese characters might take
                                         // longer to "type" due to complexity.
    {Language::KOREAN,
     0.7f * DEFAULT_ENGLISH_TYPING_SPEED},  // Korean might be slightly faster
                                            // than Japanese but slower than
                                            // Latin scripts.
    {Language::PORTUGUESE_BR, 1.0f * DEFAULT_ENGLISH_TYPING_SPEED},
    {Language::PORTUGUESE_PT, 1.0f * DEFAULT_ENGLISH_TYPING_SPEED},
    {Language::RUSSIAN,
     0.9f * DEFAULT_ENGLISH_TYPING_SPEED},  // Cyrillic script might be slightly
                                            // faster than complex scripts but
                                            // slower than Latin.
    {Language::CHINESE_SIMPLIFIED,
     0.5f * DEFAULT_ENGLISH_TYPING_SPEED},  // Chinese characters are complex
                                            // and could be set to slowest.
    {Language::CHINESE_TRADITIONAL, 0.5f * DEFAULT_ENGLISH_TYPING_SPEED}};

struct TypingEffect {
  std::u32string fullText;       // The full text to be displayed.
  size_t currentTextLength = 0;  // The length of the text to be displayed.
  float speed =
      DEFAULT_ENGLISH_TYPING_SPEED;  // The speed at which the text is typed.
  float timer = 0.f;                 // Timer to track the typing effect
  float cursorTimer = 0.f;           // Timer for cursor flashing
  float cursorInterval =
      DEFAULT_CURSOR_INTERVAL;  // The interval between cursor blinks.
  bool cursorVisible =
      DEFAULT_CURSOR_VISIBILITY;  // Whether the cursor is visible.
  bool cursorEnabled = true;      // Whether the cursor is enabled.

  bool IsCompleted() const { return currentTextLength == fullText.size(); }
  bool Update(float dt);
  std::u32string GetVisibleText() const;
};

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
       float additionalPadding = kBaseUnit / 3.0f);
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
  bool IsTypingEffectEnabled();
  void EnableTypingEffect(float speed);
  void DisableTypingEffect();
  bool UpdateTypingEffect(float dt);
  void SetLineSpacingFactor(float factor);
  void SetAdditionalPadding(float padding);
  void Draw(std::shared_ptr<TextRenderer> textRenderer,
            bool textCenteringEnabled = false, bool textRightAligned = false);

 private:
  std::vector<std::u32string> paragraphs;
  std::vector<TypingEffect> typingEffects;
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
