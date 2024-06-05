#pragma once
#include "ParticleSystem.h"
#include "SoundEngine.h"

struct ExplosionInfo {
  glm::vec2 center;
  glm::vec4 color;
  int numParticles = 150;
  float explosionPointRadiusX = 0.f;
  float explosionPointRadiusY = -1.f;
  glm::vec2 scaleRange = glm::vec2(kBubbleRadius / 45.f, kBubbleRadius / 9.f);
};

class ExplosionSystem : public ParticleSystem {
 public:
  // constructor
  ExplosionSystem(Shader shader, Texture2D texture, unsigned int amount);
  // Create explosion
  void CreateExplosion(glm::vec2 center, glm::vec4 color, int numParticles,
                       float explosionPointRadiusX = 0.f,
                       float explosionPointRadiusY = -1.f,
                       glm::vec2 scaleRange = glm::vec2(kBubbleRadius / 45.f,
                                                        kBubbleRadius / 9.f));
  // Create explosions
  void CreateExplosions(std::vector<ExplosionInfo>& explosionInfo);
  // Update all particles
  void Update(float dt);

 private:
};