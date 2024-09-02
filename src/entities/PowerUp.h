#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Bubble.h"
#include "ResourceManager.h"
#include "SoundEngine.h"

// power up state
enum class PowerUpState {
  kInactive,
  kReady,
  kActive,
  Undefined,
};

class PowerUp : public Bubble {
 public:
  PowerUp() = delete;
  PowerUp(glm::vec2 pos, float radius, Texture2D stonePlateSprite,
          Texture2D spindleSprite, Texture2D daggerSprite,
          int numOfDaggers = 0);
  PowerUp(const PowerUp& other) = default;
  PowerUp(PowerUp&& other) noexcept = default;
  ~PowerUp();

  // Setters and getters
  void SetNumOfDaggers(int numOfDaggers);
  int GetNumOfDaggers() const;

  void SetDaggerRotationSpeed(float speed);
  float GetDaggerRotationSpeed() const;

  void SetPowerUpState(PowerUpState state);
  PowerUpState GetPowerUpState() const;

  // Set position of the power up
  void SetPosition(glm::vec2 pos) override;
  // Set radius of the power up
  void SetRadius(float radius) override;

  // Move the bubble by the velocity vector. If it penetrates the wall
  // boundaries, it will bounce off. If it penetrates the top boundary.
  bool Move(
      float deltaTime, glm::vec4 boundaries,
      std::unordered_map<int, std::unique_ptr<Bubble> >& statics) override;

  bool IsStonePlateHittingBoundary() const;

  void SetIsStonePlateHittingBoundary(bool hitting);

  const std::vector<int>& GetBubblesToBeDestroyed() const;

  void Update(float dt);

  void InsertDagger();

  void Reset();

  void Draw(std::shared_ptr<Renderer> renderer) override;

 private:
  GameObject spindle, dagger;
  int numOfDaggers{0};
  float stonePlateRotationSpeed{-0.1f};
  float daggerRotationSpeed{0.1f};
  bool isStonePlateHittingBoundary{false};
  PowerUpState powerUpState{PowerUpState::kInactive};
  std::vector<int> bubblesToBeDestroyed;
};
