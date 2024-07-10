#pragma once
#include <glm/glm.hpp>

#include "GameObject.h"
#include "Timer.h"

// Scroll state
enum ScrollState {
  NARROWING,
  NARROWED,
  CLOSING,
  CLOSED,
  OPENING,
  OPENED,
  RETRACTING,
  RETRACTED,
  DEPLOYING,
  DEPLOYED,
  ATTACKING,
  ATTACKED,
  RETURNING,
  RETURNED,
  DISABLED,
};

class Scroll : public GameObject {
 public:
  Scroll(glm::vec2 center, glm::vec2 rollerSize, glm::vec2 silkSize,
         Texture2D rollerTexture);
  ~Scroll();

  // Getters and setters
  glm::vec2 GetPosition() const override;
  glm::vec2 GetCenter() const;
  glm::vec2 GetRollerSize() const;
  float GetSilkLen() const;
  float GetSilkWidth() const;
  glm::vec4 GetSilkBounds() const;
  std::unique_ptr<GameObject>& GetTopRoller();
  std::unique_ptr<GameObject>& GetBottomRoller();

  void SetSilkLen(float silkLen);
  void AddSilkLen(float delta);
  void SetCenter(glm::vec2 center) override;

  void SetState(ScrollState state);
  ScrollState GetState() const;

  void SetRoll(float roll) override;
  void SetScale(float scale) override;
  void SetAlpha(float alpha) override;

  void SetTargetSilkLenForNarrowing(float targetSilkLen);
  void SetCurrentSilkLenForNarrowing(float currentSilkLen);
  void SetTargetSilkLenForClosing(float targetSilkLen);
  void SetTargetSilkLenForOpening(float targetSilkLen);
  float GetTargetSilkLenForNarrowing() const;
  float GetCurrentSilkLenForNarrowing() const;
  float GetTargetSilkLenForClosing() const;
  float GetTargetSilkLenForOpening() const;

  void SetVelocityForNarrowing(float velocity);
  void SetVelocityForClosing(float velocity);
  void SetVelocityForOpening(float velocity);

  float GetVelocityForNarrowing() const;
  float GetVelocityForClosing() const;
  float GetVelocityForOpening() const;

  void SetTargetPositionForRetracting(glm::vec2 targetPosition);
  void SetTargetPositionForDeploying(glm::vec2 targetPosition);
  void SetTargetPositionForAttacking(glm::vec2 targetPosition);

  void SetTargetCenter(std::string target, glm::vec2 center);
  glm::vec2 GetTargetCenter(std::string target) const;

  glm::vec2 GetTargetPositionForRetracting() const;
  glm::vec2 GetTargetPositionForDeploying() const;
  glm::vec2 GetTargetPositionForAttacking() const;

  // Move the scroll based on its velocity
  void Move(float dt);

  // Move the scoll based on its velocity and the target position

  // Move the scoll based on

  // Narrow the scroll
  void Narrow(float dt, float targerSilkLen = 0.f);

  // Close the scroll
  void Close(float dt, float targerSilkLen = 0.f);

  // Open the scroll
  void Open(float dt, float targerSilkLen);

  // Retract the scroll
  void Retract(float dt);

  // Deploy the scroll
  void Deploy(float dt);

  // Attack the player
  void Attack(float dt);

  // Return the scroll to the center
  void Return(float dt);

  // Reset the scroll to the initial state
  void Reset();

  // draw scroll
  void Draw(std::shared_ptr<Renderer> renderer) override;

  static const float kSilkEdgeWidth;

 private:
  glm::vec2 center, rollerSize;
  glm::vec2 silkSize;
  std::unique_ptr<GameObject> topRoller, bottomRoller;
  std::unordered_map<std::string, glm::vec2> targetCenters;

  // Scroll state
  ScrollState state;

  // target length when narrowing
  float targetSilkLenForNarrowing;
  // cuurent length when narrowing
  float currentSilkLenForNarrowing;
  // target length when closing
  float targetSilkLenForClosing;
  // target length when opening
  float targetSilkLenForOpening;

  // narrowing velocity, always positive
  float velocityForNarrowing;
  // closing velocity, always positive
  float velocityForClosing;
  // opening velocity, always positive
  float velocityForOpening;
  // retracting/deloying/attacking/returning velocity, always positive
  float velocityForRetracting;

  glm::vec2 targetPositionForRetracting;
  glm::vec2 targetPositionForDeploying;
  glm::vec2 targetPositionForAttacking;

  // Users are not allowed to reset the position of the scroll via this
  // function.
  void SetPosition(glm::vec2 pos) override;
};