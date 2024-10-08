#pragma once
#include "Capsule.h"
#include "ScissorBoxHandler.h"
#include "Text.h"

class Health {
 public:
  // Default constructor, total health is set to 0.
  Health() = default;
  // Constructs a new Health instance. Requires the total health of the
  // character.
  explicit Health(int totalHealth);
  // Copy constructor
  Health(const Health& other);
  // Move constructor
  Health(Health&& other) noexcept = default;
  // Assignment operator
  Health& operator=(const Health& other);
  // Move assignment operator
  Health& operator=(Health&& other) noexcept = default;
  // Destructor
  ~Health() = default;
  // Getters and setters
  int GetTotalHealth() const;
  void SetTotalHealth(int totalHealth);
  int GetCurrentHealth() const;
  void SetCurrentHealth(int currentHealth);
  // Set the position, size and color of the total health bar.
  void SetTotalHealthBar(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
  //  Get the total health bar.
  Capsule& GetTotalHealthBar();
  //  Increase the current health by the given amount.
  void IncreaseHealth(int amount);
  // Derease the current health by the given amount.
  void DecreaseHealth(int amount);
  // Set the direction of the damage text.
  void SetDamagePopOutDirection(bool popOutToRight);
  // Move the newly Created damage text towards the bottom right of the health
  // bar, and then move it upwards and fade it out.
  void UpdateDamageTexts(float dt);
  // Draw the health bar. The total health is represented by an edge-only
  // capsule, and the current health is represented by a filled capsule.
  void DrawHealthBar(std::shared_ptr<ColorRenderer> colorRenderer,
                     std::shared_ptr<CircleRenderer> circleRenderer);
  // Draw the damage text.
  void DrawDamageTexts(std::shared_ptr<TextRenderer> textRenderer);

 private:
  // The total health of the character
  int totalHealth{0};
  // The current health of the character
  int currentHealth{0};
  // Capsule representing the total health
  std::unique_ptr<Capsule> totalHealthBar;
  // Color of the health bar, by default the edge is white and the fill is
  // green.
  glm::vec4 healthBarEdgeColor{1.f, 1.f, 1.f, 1.f};
  glm::vec4 healthBarFillColor{0.f, 1.f, 0.f, 1.f};
  //  Texts representing the amount of health being damaged.
  std::vector<Text> damageTexts;
  // The position where the damage text is initially created.
  glm::vec2 damageTextInitialPosition{0.f, 0.f};
  // The position where the damage text is moving towards and fading out.
  glm::vec2 damageTextTargetPosition{0.f, 0.f};
  // The position where the damage text is totally faded out.
  glm::vec2 damageTextFadedPosition{0.f, 0.f};
  // The pop out direction of the damage text.
  bool damagePopOutToRight{true};
  // The scale of the damage text.
  float damageTextScale{1.f};
  // Get the bounding box of the current health bar.
  glm::vec4 GetCurrentHealthBarBoundingBox();
};
