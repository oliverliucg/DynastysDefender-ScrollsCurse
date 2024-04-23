#include "ShadowTrailSystem.h"

ShadowTrailSystem::ShadowTrailSystem(Shader shader, Texture2D texture, unsigned int amount) : ParticleSystem(shader, texture, amount) {}


void ShadowTrailSystem::Update(float dt)
{
    // update all particles
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        Particle& p = this->particles[i];
        p.lifespan -= dt; // reduce life
        if (p.lifespan > 0.0f)
        {	// particle is alive, thus update
            p.position += p.velocity * dt;
            // fade out
            p.color.a -= dt * p.fadeOutSpeed;
            if (p.color.a < 0.0f)
                p.color.a = 0.0f;
        }
    }
}

void ShadowTrailSystem::respawnParticles(GameObject& object, int numParticles, glm::vec2 velocity, glm::vec2 scaleRange, glm::vec2 offset)
{
    for (unsigned int i = 0; i < numParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        float randomMaxScale = 2.5f;
        // Create a random X that is from -randomMaxScale * kBubbleRadius to randomMaxScale * kBubbleRadius
        float randomX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * randomMaxScale * 2 * kBubbleRadius - randomMaxScale * kBubbleRadius;
        // Create a random Y that is from -randomMaxScale * kBubbleRadius to randomMaxScale * kBubbleRadius
        float randomY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * randomMaxScale * 2 * kBubbleRadius - randomMaxScale * kBubbleRadius;
        float rColor = 0.5f + ((rand() % 100) / 100.0f);
        this->particles[unusedParticle].position = object.GetPosition() + glm::vec2(randomX, randomY) + offset;
       /* this->particles[unusedParticle].position = object.GetPosition();*/
        this->particles[unusedParticle].color = glm::vec4(rColor, rColor, rColor, 1.0f);
       /* glm::vec4 color = glm::vec4(0.3216f,0.2941f,0.2431f,1.f);*/
       /* this->particles[unusedParticle].color = slightlyVaryColor(color);*/
        this->particles[unusedParticle].lifespan = 1.0f;
        this->particles[unusedParticle].velocity = velocity;
        this->particles[unusedParticle].scale = getRandomScale(scaleRange);
        this->particles[unusedParticle].fadeOutSpeed = 0.6f*glm::length(velocity)/kBubbleRadius+0.4f;
        //if (this->particles[unusedParticle].position.y >= 0.f) {
        //    std::cout << "unused particle: " << unusedParticle << std::endl;
        //    std::cout << "position: " << this->particles[unusedParticle].position.x << ", " << this->particles[unusedParticle].position.y << std::endl;
        //    std::cout << "color: " << rColor << std::endl;
        //    std::cout << "lifespan: " << this->particles[unusedParticle].lifespan << std::endl;
        //    std::cout << "velocity: " << this->particles[unusedParticle].velocity.x << ", " << this->particles[unusedParticle].velocity.y << std::endl;
        //    std::cout << "scale: " << this->particles[unusedParticle].scale << std::endl;
        //}
    }
}