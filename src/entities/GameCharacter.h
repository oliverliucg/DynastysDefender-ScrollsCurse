#pragma once
#include "GameObject.h"
#include "Health.h"

// State of the character
enum class GameCharacterState {
  kNormal,
  HAPPY,
  SAD,
  ANGRY,
  FIGHTING,
  WINNING,
};

class GameCharacter : public GameObject {
 public:
  // Default constructor is deleted
  GameCharacter() = delete;
  // Constructs a new Character instance. Requires the name of the character,
  // the state of the character, a hashmap of positions for each state, a
  // hashmpa of sizes for each state, a rotation angle, a pivot vector, and a
  // hashmap of textures for each state.
  GameCharacter(const std::string& name, GameCharacterState state,
                std::unordered_map<GameCharacterState, glm::vec2> pos,
                std::unordered_map<GameCharacterState, glm::vec2> size,
                float rotation, glm::vec2 pivot,
                std::unordered_map<GameCharacterState, Texture2D> textures);
  // Copy constructor is deleted
  GameCharacter(const GameCharacter& other);
  // Move constructor
  GameCharacter(GameCharacter&& other) noexcept;
  // Assignment operator is deleted
  GameCharacter& operator=(const GameCharacter& other);
  // Move assignment operator
  GameCharacter& operator=(GameCharacter&& other) noexcept;
  ~GameCharacter();

  // Draw the character
  void DrawGameCharacter(std::shared_ptr<SpriteRenderer> sprideRenderer,
                         std::shared_ptr<ColorRenderer> colorRenderer,
                         std::shared_ptr<CircleRenderer> circleRenderer,
                         std::shared_ptr<TextRenderer> textRenderer);

  // Getters and setters
  std::string GetName() const;
  GameCharacterState GetState() const;
  void SetState(GameCharacterState state);
  Health& GetHealth();
  void SetAndMoveToTargetPosition(glm::vec2 position);
  glm::vec2 GetCurrentTargetPosition() const;
  void SetTargetRoll(float roll);
  float GetTargetRoll() const;

  // Set target position
  void SetTargetPosition(const std::string& targetName, glm::vec2 position);

  // Get target position
  glm::vec2 GetTargetPosition(const std::string& targetName) const;

  // Get position ratio of the carried objects. Key is the object id, value is
  // the relative position ratio.
  std::unordered_map<int, std::unordered_map<GameCharacterState, glm::vec2>>&
  GetRelativeCenterRatios();

  // Set the face direction of the character
  void SetFaceDirection(bool faceLeft);

  // Get reference to the carried objects
  std::unordered_map<int, std::shared_ptr<GameObject>>& GetCarriedObjects();

  // Add an object to the carried objects
  void AddCarriedObject(std::shared_ptr<GameObject> object);

  void ActivateStun();
  void DeactivateStun();
  bool IsMoving() const;
  bool IsRotating() const;
  bool IsStunned() const;

  // Set the position of the character
  void SetPosition(glm::vec2 position) override;

  // Set transparency of the character
  void SetAlpha(float alpha) override;

  // method to move the character to a certain position with a certain velocity.
  // The velocity should be a positive number.
  void MoveTo(glm::vec2 position, float velocity, float dt);

  // method to move the character to a certain position with a certain velocity
  // and acceleration. The velocity should be positive numbers.
  void MoveTo(glm::vec2 position, float velocity, float acceleration, float dt);

  // method to rotate the character to a certain angle with a certain angular
  // velocity. The angular velocity can be a positive or negative number.
  void RotateTo(float angle, float angularVelocity, float dt);

  // Set the carried object's relative position ratio given the object center.
  void SetRelativeCenterRatio(GameCharacterState state, int id,
                              glm::vec2 center);

  // Set the relative position ratio of the health bar if the character has a
  // health bar.
  void SetHealthBarRelativeCenterRatios();

  // Clear carried objects and relative position ratios
  void ClearCarriedObjects();

  // Get the size of the character under certain state
  glm::vec2 GetGeneralSize(GameCharacterState state) const;

  // Get the texture of the character under certain state
  Texture2D GetGeneralTexture(GameCharacterState state) const;

  // Set textures of the character
  void SetTextures(
      const std::unordered_map<GameCharacterState, Texture2D>& textures);

 private:
  std::string name;
  GameCharacterState state{};
  std::unique_ptr<Health> health;
  std::unordered_map<GameCharacterState, glm::vec2> positions;
  std::unordered_map<GameCharacterState, glm::vec2> sizes;
  std::unordered_map<GameCharacterState, Texture2D> textures;
  std::unordered_map<int, std::shared_ptr<GameObject>> carriedObjects;
  std::unordered_map<std::string, glm::vec2> targetPositions;
  // Relative position ratio of the carried objects and the health bar. Ket is
  // the object id, value is the relative position ratio.
  std::unordered_map<int, std::unordered_map<GameCharacterState, glm::vec2>>
      relativeCenterRatios;
  glm::vec2 targetPosition{0.f, 0.f};
  float targetRoll{0.f};
  bool faceLeft{false};
  bool isMoving{false};
  bool isRotating{false};
  bool isStunned{false};

  // Update the position of the carried objects based on the character's
  // position and the relative position ratio.
  void UpdateCarriedObjectsPosition();

  // Update the position of the health bar based on the character's position and
  // the relative position ratio.
  void UpdateHealthBarPosition();
};
