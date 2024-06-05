#include "ExplosionSystem.h"

ExplosionSystem::ExplosionSystem(Shader shader, Texture2D texture,
                                 unsigned int amount)
    : ParticleSystem(shader, texture, amount) {}

void ExplosionSystem::CreateExplosion(glm::vec2 center, glm::vec4 color,
                                      int numParticles,
                                      float explosionPointRadiusX,
                                      float explosionPointRadiusY,
                                      glm::vec2 scaleRange) {
  for (unsigned int i = 0; i < numParticles; ++i) {
    int unusedParticle = this->firstUnusedParticle();
    this->particles[unusedParticle].position = center;
    if (explosionPointRadiusX > 0.f) {
      if (explosionPointRadiusY < 0.f) {
        // Get a random position from the center of the explosion within a
        // circle with radius explosionPointRadiusX
        float angle = static_cast<float>(rand()) /
                      static_cast<float>(RAND_MAX) * 2 * glm::pi<float>();
        float radius = static_cast<float>(rand()) /
                       static_cast<float>(RAND_MAX) * explosionPointRadiusX;
        glm::vec2 offset =
            glm::vec2(radius * std::cos(angle), radius * std::sin(angle));
        this->particles[unusedParticle].position += offset;
      } else {
        if (explosionPointRadiusY == 0.f) {
          // Get a random position from the center of the explosion within a
          // line with length explosionPointRadiusX The offset can be either
          // positive or negative
          float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) *
                        2 * explosionPointRadiusX -
                    explosionPointRadiusX;
          glm::vec2 offset = glm::vec2(x, 0.f);
          this->particles[unusedParticle].position += offset;
        } else {
          // Get a random position from the center of the explosion within a
          // rectangle with width 2*explosionPointRadiusX and height
          // 2*explosionPointRadiusY The offset can be either positive or
          // negative
          float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) *
                        2 * explosionPointRadiusX -
                    explosionPointRadiusX;
          float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) *
                        2 * explosionPointRadiusY -
                    explosionPointRadiusY;
          glm::vec2 offset = glm::vec2(x, y);
          this->particles[unusedParticle].position += offset;
        }
      }
    }
    this->particles[unusedParticle].velocity = getRandomVelocity();
    this->particles[unusedParticle].color = slightlyVaryColor(color);
    this->particles[unusedParticle].lifespan = getRandomLifespan();
    this->particles[unusedParticle].scale = getRandomScale(scaleRange);
  }
}

void ExplosionSystem::CreateExplosions(
    std::vector<ExplosionInfo>& explosionInfo) {
  // add new particles
  for (const auto& info : explosionInfo) {
    glm::vec2 center = info.center;
    glm::vec4 color = info.color;
    int numParticles = info.numParticles;
    float explosionPointRadiusX = info.explosionPointRadiusX;
    float explosionPointRadiusY = info.explosionPointRadiusY;
    glm::vec2 scaleRange = info.scaleRange;
    CreateExplosion(center, color, numParticles, explosionPointRadiusX,
                    explosionPointRadiusY, scaleRange);
  }

  // Play explosion sound
  SoundEngine::GetInstance().PlaySound("bubbleexplode");
}

void ExplosionSystem::Update(float dt) {
  // update all particles
  for (unsigned int i = 0; i < this->amount; ++i) {
    Particle& p = this->particles[i];
    p.lifespan -= dt;  // reduce life
    if (p.lifespan > 0.0f) {
      // particle is alive, thus update
      p.position += p.velocity * dt;
      // apply gravity
      p.velocity.y += 9.8f * kVelocityUnit * dt;
      // fade out
      p.color.a -= dt * 0.4f;
      if (p.color.a < 0.0f) p.color.a = 0.0f;
    }
  }
}