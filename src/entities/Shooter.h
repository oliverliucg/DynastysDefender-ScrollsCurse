#pragma once
#include <glm/gtc/constants.hpp>  // For glm::pi<float>()
#include <random>
#include <unordered_set>

#include "Bubble.h"
#include "GameObject.h"
#include "Ray.h"
#include "ResourceManager.h"
#include "SoundEngine.h"
// The Shooter class is a GameObject that is controlled by the player. It is the
// object that shoots the bubbles. It carries a bubble that it will shoot when
// the player presses the space bar. it also carries another that it's going to
// shoot next.
class Shooter : public GameObject {
 public:
  // Constructs a new Shooter instance. Requires a position vector, size vector,
  // rotation pivot vector, and a sprite. Will Initialize the carriedBubble
  // object at it rotation center and nextBubble object at its tail.
  Shooter(glm::vec2 pos, glm::vec2 size, glm::vec2 rotationPivot,
          glm::vec2 tailPivot, Texture2D sprite);
  ~Shooter() = default;

  void Draw(std::shared_ptr<Renderer> renderer);

  Ray& GetRay();

  // Shoots the bubble that the shooter is carrying.
  // Returns a unique pointer to the bubble.
  std::unique_ptr<Bubble> ShootBubble();

  // Refresh the color of the carried bubble.
  void RefreshCarriedBubbleColor(glm::vec3 color);

  // Refresh the color of the next bubble.
  void RefreshNextBubbleColor(glm::vec3 color);

  // Shoots the bubble that the shooter is carrying.
  // Returns a unique pointer to the bubble.
  std::unique_ptr<Bubble> ShootBubble(glm::vec4 nextBubbleColor);

  // Generates a new bubble for the shooter to carry.
  void GenerateBubble();

  // Set roll for the shooter and the next bubble.
  void SetRoll(float roll);

  // Set the position of the shooter.
  void SetPosition(glm::vec2 pos);

  // Get the const bubble that the shooter is carrying
  const Bubble& GetCarriedBubble() const;

  // Get a new color for the next bubble based on the exsiting static bubbles.
  glm::vec4 GetNewBubbleColor(
      const std::unordered_map<int, std::unique_ptr<Bubble>>& statics);

  // Update carriedBubble radius.
  void UpdateCarriedBubbleRadius(float radius);

  // Swap the carried bubble and the next bubble.
  void SwapCarriedBubbleAndNextBubble();

 private:
  // The bubble that the shooter is carrying.
  Bubble carriedBubble;
  // Anoter bubble that the shooter is going to shoot next.
  Bubble nextBubble;
  // The ray start from the center carriedBubble.
  Ray ray;
  //  Get the shooting direction.
  glm::vec2 GetShootingDirection();
  // Get a new color for the next bubble.
  glm::vec4 GetNewBubbleColor();
};