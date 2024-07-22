/*
 * ShadowTrailSystem.cpp
 * Copyright (C) 2024 Oliver Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "ShadowTrailSystem.h"

ShadowTrailSystem::ShadowTrailSystem(Shader shader, Texture2D texture,
                                     unsigned int amount)
    : ParticleSystem(shader, texture, amount) {}

void ShadowTrailSystem::Update(float dt) {
  // update all particles
  for (unsigned int i = 0; i < this->amount; ++i) {
    Particle& p = this->particles[i];
    p.lifespan -= dt;         // reduce life
    if (p.lifespan > 0.0f) {  // particle is alive, thus update
      p.position += p.velocity * dt;
      // fade out
      p.color.a -= dt * p.fadeOutSpeed;
      if (p.color.a < 0.0f) p.color.a = 0.0f;
    }
  }
}

void ShadowTrailSystem::respawnParticles(GameObject& object, int numParticles,
                                         glm::vec2 velocity,
                                         glm::vec2 scaleRange,
                                         glm::vec2 offset) {
  for (unsigned int i = 0; i < numParticles; ++i) {
    int unusedParticle = this->firstUnusedParticle();
    float randomMaxScale = 2.5f;
    // Create a random X that is from -randomMaxScale * kBaseUnit to
    // randomMaxScale * kBaseUnit
    float randomX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) *
                        randomMaxScale * 2 * kBaseUnit -
                    randomMaxScale * kBaseUnit;
    // Create a random Y that is from -randomMaxScale * kBaseUnit to
    // randomMaxScale * kBaseUnit
    float randomY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) *
                        randomMaxScale * 2 * kBaseUnit -
                    randomMaxScale * kBaseUnit;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    this->particles[unusedParticle].position =
        object.GetPosition() + glm::vec2(randomX, randomY) + offset;
    /* this->particles[unusedParticle].position = object.GetPosition();*/
    this->particles[unusedParticle].color =
        glm::vec4(rColor, rColor, rColor, 1.0f);
    /* glm::vec4 color = glm::vec4(0.3216f,0.2941f,0.2431f,1.f);*/
    /* this->particles[unusedParticle].color = slightlyVaryColor(color);*/
    this->particles[unusedParticle].lifespan = 1.0f;
    this->particles[unusedParticle].velocity = velocity;
    this->particles[unusedParticle].scale = getRandomScale(scaleRange);
    this->particles[unusedParticle].fadeOutSpeed =
        0.6f * glm::length(velocity) / kBaseUnit + 0.4f;
  }
}