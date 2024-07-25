#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>

#include "GameObject.h"
#include "ResourceManager.h"

// Bubbles Colors in this game:
//	Red:    (255, 0, 0)
//	Green:  (0, 255, 0)
//	Blue:   (0, 0, 255)
//	Yellow: (255, 255, 0)
//	Purple: (128, 0, 128)
//	Orange: (255, 165, 0)

// State of the bubble

enum class BubbleState {
  kNormal,
  Moving,
  Static,
  Falling,
  Exploding,
  Undefined,
};

class Bubble : public GameObject {
 public:
  // Default constructor
  Bubble();
  // Constructs a new Bubble instance. Requires a position vector, radius,
  // velocity vector, and color vector.
  Bubble(glm::vec2 pos, float radius, glm::vec2 velocity, glm::vec4 color,
         Texture2D sprite);
  // Copy constructor
  Bubble(const Bubble& other);
  // Move constructor
  Bubble(Bubble&& other) noexcept;
  // Assignment operator
  Bubble& operator=(const Bubble& other);
  // Move assignment operator
  Bubble& operator=(Bubble&& other) noexcept;

  // Move the bubble by the velocity vector. If it penetrates the left, right,
  // or bottom boundaries, it will bounce off. If it penetrates the top boundary
  // or it penetrates the static bubbles, its position will be adjusted to the
  // point of penetration and its velocity will be set to 0.
  bool Move(float deltaTime, glm::vec4 boundaries,
            std::unordered_map<int, std::unique_ptr<Bubble> >& statics);

  // Move the bubble by the velocity vector. No boundary check.
  void Move(float deltaTime);

  // Getters and setters
  void SetRadius(float radius);
  float GetRadius() const;
  glm::vec2 GetCenter() const;

  // Apply gravity to the bubble.
  void ApplyGravity(float deltaTime);

 private:
  float radius{0.f};
  BubbleState state{BubbleState::kNormal};
};