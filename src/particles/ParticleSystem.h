#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <tuple>
#include <vector>

#include "GameObject.h"
#include "Shader.h"
#include "Texture.h"

// Represents a single particle and its state
struct Particle {
  glm::vec2 position{0.0f, 0.0f}, velocity{0.0f, 0.0f};
  glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
  bool isDeepColor{false};
  float lifespan{0.0f};
  float scale{1.0f};
  float fadeOutSpeed{1.0f};
  // default constructor
  Particle() = default;
};

// ParticleSystem acts as a container for rendering a large number of
// particles by repeatedly spawning and updating particles and killing
// them after a given amount of time.
class ParticleSystem {
 public:
  // constructor
  ParticleSystem(Shader shader, Texture2D texture, unsigned int amount);
  // Update all particles
  virtual void Update(float dt);
  // render all particles
  void Draw(bool isDarkBackground = true);

  // Set shader of the particle system
  void SetShader(Shader shader);

  // Set texture of the particle system
  void SetTexture(Texture2D texture);

 protected:
  // state
  std::vector<Particle> particles;
  unsigned int amount{3000};
  // render state
  Shader shader;
  Texture2D texture;
  unsigned int VAO{0};
  // initializes buffer and vertex attributes
  void init();
  // Slightly varies a color component
  glm::vec4 slightlyVaryColor(glm::vec4 color);
  // returns the first Particle index that's currently unused e.g. Life <= 0.0f
  // or 0 if no particle is currently inactive
  unsigned int firstUnusedParticle();
  // Get a random velocity
  glm::vec2 getRandomVelocity();
  // Get a random lifespan
  float getRandomLifespan();
  // Get a random scale
  float getRandomScale(glm::vec2 scaleRange = glm::vec2(1.f, 5.f));
};