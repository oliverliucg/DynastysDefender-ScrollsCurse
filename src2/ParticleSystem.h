#pragma once
#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"


// Represents a single particle and its state
struct Particle {
    glm::vec2 position, velocity;
    glm::vec4 color;
    float lifespan;
    float scale;
    float fadeOutSpeed;
    // default constructor
    Particle() : position(0.0f), velocity(0.0f), color(1.0f), lifespan(0.0f), scale(1.0f), fadeOutSpeed(1.0f) { }
};


// ParticleSystem acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleSystem
{
public:
    // constructor
    ParticleSystem(Shader shader, Texture2D texture, unsigned int amount);
    // Update all particles
    virtual void Update(float dt);
    // render all particles
    void Draw(bool isDarkBackground = true);
    
protected:
    // state
    std::vector<Particle> particles;
    unsigned int amount;
    // render state
    Shader shader;
    Texture2D texture;
    unsigned int VAO;
    // initializes buffer and vertex attributes
    void init();
    // Slightly varies a color component
    glm::vec4 slightlyVaryColor(glm::vec4 color);
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // Get a random velocity
    glm::vec2 getRandomVelocity();
    // Get a random lifespan
    float getRandomLifespan();
    // Get a random scale
    float getRandomScale(glm::vec2 scaleRange = glm::vec2(1.f, 5.f));
};