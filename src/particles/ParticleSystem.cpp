#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(Shader shader, Texture2D texture,
                               unsigned int amount)
    : shader(shader), texture(texture), amount(amount) {
  this->init();
}

void ParticleSystem::Update(float dt) {}

// render all particles
void ParticleSystem::Draw(bool isDarkBackground) {
  // Get the original blending function parameters
  GLint originalSrc, originalDst;
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &originalSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &originalDst);

  // use additive blending to give it a 'glow' effect
  if (isDarkBackground) glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  this->shader.Use();
  for (auto& particle : this->particles) {
    if (particle.lifespan > 0.0f) {
      bool isDeepColor = particle.isDeepColor;
      this->shader.SetFloat("scale", particle.scale);
      this->shader.SetVector2f("offset", particle.position);
      glm::vec4 particleColor = particle.color;
      if (!isDeepColor && !isDarkBackground) {
        // glm::vec3 adjustedColor = adjustColorForBrightBackground(
        //     glm::vec3(particleColor.x, particleColor.y, particleColor.z));
        // particleColor = glm::vec4(adjustedColor, particleColor.w);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        // int src, dst;
        // glGetIntegerv(GL_BLEND_SRC_ALPHA, &src);
        // glGetIntegerv(GL_BLEND_DST_ALPHA, &dst);
      }
      this->shader.SetVector4f("color", particleColor);
      this->texture.Bind();
      glBindVertexArray(this->VAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
      if (!isDeepColor && !isDarkBackground) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
    }
  }

  // Reset to default blending mode
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Restore the original blend function
  if (isDarkBackground) glBlendFunc(originalSrc, originalDst);
}

void ParticleSystem::init() {
  // set up mesh and attribute properties
  unsigned int VBO;
  float particle_quad[] = {
      0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

      0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(this->VAO);
  // fill mesh buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad,
               GL_STATIC_DRAW);
  // set mesh attributes
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glBindVertexArray(0);

  // create this->amount default particle instances
  for (unsigned int i = 0; i < this->amount; ++i)
    this->particles.push_back(Particle());
}

// stores the index of the last particle used (for quick access to next dead
// particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleSystem::firstUnusedParticle() {
  // first search from last used particle, this will usually return almost
  // instantly
  for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
    if (this->particles[i].lifespan <= 0.0f) {
      lastUsedParticle = i;
      return i;
    }
  }
  // otherwise, do a linear search
  for (unsigned int i = 0; i < lastUsedParticle; ++i) {
    if (this->particles[i].lifespan <= 0.0f) {
      lastUsedParticle = i;
      return i;
    }
  }
  // all particles are taken, override the first one (note that if it repeatedly
  // hits this case, more particles should be reserved)
  lastUsedParticle = 0;
  return 0;
}

glm::vec4 ParticleSystem::slightlyVaryColor(glm::vec4 color) {
  // alpha remains the same

  float a = 1.0;
  return glm::vec4(color.r, color.g, color.b, a);

  // Define the maximum variation you want in each color component.
  // For example, +/- 0.05 ensures the color variation is subtle.
  float variationRange = 0.05f;

  // Adjust each color component by a random amount within the variation range.
  float newR = glm::clamp(
      color.r + glm::linearRand(-variationRange, variationRange), 0.0f, 1.0f);
  float newG = glm::clamp(
      color.g + glm::linearRand(-variationRange, variationRange), 0.0f, 1.0f);
  float newB = glm::clamp(
      color.b + glm::linearRand(-variationRange, variationRange), 0.0f, 1.0f);

  // Keep the alpha value the same as the input color.
  float newA = color.a;

  return glm::vec4(newR, newG, newB, newA);
}

glm::vec2 ParticleSystem::getRandomVelocity() {
  // Get a random velocity from 0 to 8*kVelocityUnit, the difference between
  // random velocities is 0.5*kVelocityUnit
  float velocity = 0.5 * (rand() % 16) * kVelocityUnit;
  float angle = (rand() % 360) * (glm::pi<float>() / 180.0f);
  float vx = velocity * cos(angle);
  float vy = velocity * sin(angle);
  return glm::vec2(vx, vy);
}

float ParticleSystem::getRandomLifespan() {
  // Get a random lifespan from 1.0 to 3.0
  return 1.0f + (rand() % 200) / 100.0f;
}

float ParticleSystem::getRandomScale(glm::vec2 scaleRange) {
  // Get a random scale from scaleRange.x to scaleRange.y
  return generateRandom(scaleRange.x, scaleRange.y);
}

void ParticleSystem::SetShader(Shader shader) { this->shader = shader; }

void ParticleSystem::SetTexture(Texture2D texture) { this->texture = texture; }