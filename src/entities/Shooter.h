#pragma once
#include <glm/gtc/constants.hpp>  // For glm::pi<float>()
#include <random>
#include <unordered_set>

#include "Bubble.h"
#include "GameObject.h"
#include "PowerUp.h"
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

  //// Shoots the bubble that the shooter is carrying.
  //// Returns a unique pointer to the bubble.
  // std::unique_ptr<Bubble> ShootBubble();

  // Refreshes the color of the carried bubble.
  void RefreshCarriedBubbleColor(glm::vec3 color);

  // Refreshes the color of the next bubble.
  void RefreshNextBubbleColor(glm::vec3 color);

  // Equips the power up to the shooter.
  void EquipPowerUp(std::unique_ptr<PowerUp> powerUp);

  // Shoots the bubble that the shooter is carrying.
  // Returns a unique pointer to the bubble.
  std::unique_ptr<Bubble> ShootBubble(glm::vec4 nextBubbleColor);

  // Generates a new bubble for the shooter to carry.
  void GenerateBubble();

  // Sets roll for the shooter and the next bubble.
  void SetRoll(float roll);

  // Sets the position of the shooter.
  void SetPosition(glm::vec2 pos);

  // Gets the const bubble that the shooter is carrying
  const Bubble& GetCarriedBubble() const;

  // Gets the const bubble that the shooter is going to shoot next.
  const Bubble& GetNextBubble() const;

  // Gets a new color for the next bubble based on the exsiting static bubbles.
  glm::vec4 GetNewBubbleColor(
      const std::unordered_map<int, std::unique_ptr<Bubble>>& statics);

  // Checks if the shooter is carrying a power up.
  bool HasPowerUp() const;

  // Updates the powerup carried by the shooter.
  void UpdatePowerUp(float dt);

  // Updates carriedBubble radius.
  void UpdateCarriedBubbleRadius(float radius);

  // Swaps the carried bubble and the next bubble.
  void SwapCarriedBubbleAndNextBubble();

 private:
  // The bubble that the shooter is carrying.
  std::unique_ptr<Bubble> carriedBubble;
  // Anoter bubble that the shooter is going to shoot next.
  std::unique_ptr<Bubble> nextBubble;
  // The ray start from the center carriedBubble.
  Ray ray;
  // Gets the shooting direction.
  glm::vec2 GetShootingDirection() const;
  // Gets a new color for the next bubble.
  glm::vec4 GetNewBubbleColor() const;
};