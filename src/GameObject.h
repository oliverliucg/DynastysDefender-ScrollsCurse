#pragma once
#include <glad/glad.h>

#include <cassert>
#include <glm/glm.hpp>
#include <memory>

#include "ColorRenderer.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SpriteDynamicRenderer.h"
#include "SpriteRenderer.h"
#include "Texture.h"
// GameObject contains all state information relevant to a game object as shown
// by its variable names.
class GameObject {
 public:
  // Constructor(s)
  GameObject();
  GameObject(glm::vec2 pos, glm::vec2 size, float roll = 0.0f,
             glm::vec2 rotationPivot = glm::vec2(0.5f, 0.5f),
             glm::vec2 velocity = glm::vec2(0.0f, 0.0f),
             glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
             Texture2D sprite = Texture2D());
  // Copy constructor
  GameObject(const GameObject& other);
  // Move constructor
  GameObject(GameObject&& other) noexcept;
  // Copy assignment
  GameObject& operator=(const GameObject& other);
  // Move assignment
  GameObject& operator=(GameObject&& other) noexcept;

  // Destructor
  ~GameObject();

  // Getters and setters
  virtual glm::vec2 GetPosition() const;
  virtual glm::vec2 GetCenter() const;
  glm::vec2 GetSize() const;
  float GetScale() const;
  float GetRoll() const;
  glm::vec2 GetRotationPivot() const;
  glm::vec2 GetRotationPivotCoord() const;
  glm::vec2 GetVelocity() const;
  glm::vec2 GetAcceleration() const;
  glm::vec4 GetColor() const;
  glm::vec3 GetColorWithoutAlpha() const;
  float GetAlpha() const;
  Color GetColorEnum() const;
  int GetID() const;

  virtual void SetPosition(glm::vec2 pos);
  virtual void SetCenter(glm::vec2 center);
  void SetSize(glm::vec2 size);
  virtual void SetScale(float scale);
  virtual void SetRoll(float roll);
  void SetRotationPivot(glm::vec2 rotationPivot);
  void SetVelocity(glm::vec2 velocity);
  void SetAcceleration(glm::vec2 acceleration);
  void SetColor(glm::vec4 color);
  void SetColorWithoutAlpha(glm::vec3 color);
  virtual void SetAlpha(float alpha);
  void SetTexture(Texture2D sprite);

  virtual void StopMoving();

  // draw sprite
  virtual void Draw(std::shared_ptr<Renderer> renderer);
  // draw sprite dynamically
  virtual void Draw(std::shared_ptr<SpriteDynamicRenderer> renderer,
                    glm::vec4 texCoords);

 protected:
  // Object state
  float roll, scale;
  glm::vec2 position, size, rotationPivot, velocity, acceleration;
  // color with alpha (RGBA)
  glm::vec4 color;
  // render state
  Texture2D sprite;
  int uniqueID;
};