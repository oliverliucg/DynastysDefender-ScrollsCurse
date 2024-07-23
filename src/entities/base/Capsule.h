#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "CircleRenderer.h"
#include "ColorRenderer.h"
#include "GameObject.h"
#include "LineRenderer.h"

// A capsule has two semicircles and a rectangle.
class Capsule : public GameObject {
 public:
  // Default constructor
  Capsule() = delete;
  // Default destructor
  ~Capsule() = default;
  // Constructs a new Capsule instance.
  Capsule(glm::vec2 center, glm::vec2 size, glm::vec4 color);
  // Copy constructor
  Capsule(const Capsule& other);
  // Assignment operator
  Capsule& operator=(const Capsule& other);
  // Getters and setters for the capsule.
  glm::vec2 GetCenter() const override;
  void SetCenter(glm::vec2 center) override;

  // Get the radius of the semicircles.
  float GetRadius() const;
  // Get the size of the rectangle.
  glm::vec2 GetRectangleSize() const;

  // Get the center of the top semicircles.
  glm::vec2 GetTopSemiCircleCenter() const;
  // Get the center of the bottom semicircles.
  glm::vec2 GetBottomSemiCircleCenter() const;
  // Get the top left corner of the rectangle.
  glm::vec2 GetRectangleTopLeft() const;
  // Set the visibility of the rectangle.
  void SetRectangleVisible(bool visible);
  // Set the visibility of the top semicircle.
  void SetTopSemiCircleVisible(bool visible);
  // Set the visibility of the bottom semicircle.
  void SetBottomSemiCircleVisible(bool visible);
  // Get the visibility of the rectangle.
  bool IsRectangleVisible() const;
  // Get the visibility of the top semicircle.
  bool IsTopSemiCircleVisible() const;
  // Get the visibility of the bottom semicircle.
  bool IsBottomSemiCircleVisible() const;

  // Move the capsule based on the given offset and the boundaries.
  void Move(glm::vec2 offset, glm::vec4 boundaries);

  // Detect if the given position is inside the capsule.
  bool IsPositionInside(glm::vec2 mousePosition);

  // Draw the capsule. colorRenderer is used to draw the rectangle and
  // circleRenderer is used to draw the semicircles.
  void Draw(std::shared_ptr<ColorRenderer> colorRenderer,
            std::shared_ptr<CircleRenderer> circleRenderer);

  // Draw the outline of the capsule. ColorRenderer is used to draw the
  // rectangle's outline and CircleRenderer is used to draw the semicircles.
  void DrawOutline(std::shared_ptr<ColorRenderer> colorRenderer,
                   std::shared_ptr<CircleRenderer> circleRenderer);

 private:
  glm::vec2 center;
  bool rectangleVisible, topSemiCircleVisible, bottomSemiCircleVisible;
};
