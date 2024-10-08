#pragma once

#include "GameObject.h"
#include "SoundEngine.h"

class Arrow : public GameObject {
 public:
  // Default constructor is deleted
  Arrow() = delete;
  // Constructor
  Arrow(glm::vec2 pos, glm::vec2 size, Texture2D sprite);
  // Copy constructor
  Arrow(const Arrow& other);
  // Move constructor
  Arrow(Arrow&& other);
  // Copy assignment
  Arrow& operator=(const Arrow& other);
  // Move assignment
  Arrow& operator=(Arrow&& other);
  // Destructor
  ~Arrow();
  // Fire the arrow
  void Fire(glm::vec2 target, float speed);
  // Update the arrow
  void Update(float dt);
  // Set hit status
  void SetHit(bool hit);
  // Check if the arrow is fired
  bool IsFired();
  // Check if the arrow is stopped
  bool IsStopped();
  // Check if the arrow is penetrating
  bool IsPenetrating();
  // Get the penetration position
  glm::vec2 GetPenetrationPosition();
  // Get texture coordinates
  glm::vec4 GetTextureCoords();
  //// Draw the arrow
  // void Draw(std::shared_ptr<Renderer> renderer);
 private:
  bool isFired{false};
  bool isStopped{false};
  bool isPenetrating{false};
  float speed{0.f};
  float penetrationRatio{0.f};
  glm::vec2 penetrationPosition{0.f, 0.f};
  glm::vec2 targetPosition{0.f, 0.f};

  // Get the direction that the arrow points to
  glm::vec2 GetArrowHeadDirection();

  //// Get texture coordinates
  // glm::vec4 GetTextureCoords();
};
