#pragma once
#include "ParticleSystem.h"

class ShadowTrailSystem : public ParticleSystem {
 public:
  // constructor
  ShadowTrailSystem(Shader shader, Texture2D texture, unsigned int amount);

  // Update all particles
  void Update(float dt);
  // respawns particle
  void respawnParticles(GameObject& object, int numParticles,
                        glm::vec2 velocity = glm::vec2(0.f),
                        glm::vec2 scaleRange = glm::vec2(10.f, 10.f),
                        glm::vec2 offset = glm::vec2(0.0f, 0.0f));

 private:
};