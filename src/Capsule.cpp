#include "Capsule.h"

Capsule::Capsule(glm::vec2 center, glm::vec2 size, glm::vec4 color)
    : GameObject(center - size / 2.0f, size, 0.0f, glm::vec2(0.5f, 0.5f),
                 glm::vec2(0.0f, 0.0f), color),
      center(center),
      rectangleVisible(true),
      topSemiCircleVisible(true),
      bottomSemiCircleVisible(true) {}

Capsule::Capsule(const Capsule& other)
    : GameObject(other),
      center(other.center),
      rectangleVisible(other.rectangleVisible),
      topSemiCircleVisible(other.topSemiCircleVisible),
      bottomSemiCircleVisible(other.bottomSemiCircleVisible) {}

Capsule& Capsule::operator=(const Capsule& other) {
  if (this == &other) {
    return *this;
  }
  GameObject::operator=(other);
  center = other.center;
  rectangleVisible = other.rectangleVisible;
  topSemiCircleVisible = other.topSemiCircleVisible;
  bottomSemiCircleVisible = other.bottomSemiCircleVisible;
  return *this;
}

glm::vec2 Capsule::GetCenter() const { return center; }

void Capsule::SetCenter(glm::vec2 center) {
  this->center = center;
  SetPosition(center - GetSize() / 2.0f);
}

float Capsule::GetRadius() const { return size.x / 2.0f; }

glm::vec2 Capsule::GetTopSemiCircleCenter() const {
  float height = size.y - size.x;
  return glm::vec2(center.x, center.y - height / 2.0f);
}

glm::vec2 Capsule::GetBottomSemiCircleCenter() const {
  float height = size.y - size.x;
  return glm::vec2(center.x, center.y + height / 2.0f);
}

glm::vec2 Capsule::GetRectangleTopLeft() const {
  float height = size.y - size.x;
  return glm::vec2(center.x - size.x / 2.0f, center.y - height / 2.0f);
}

glm::vec2 Capsule::GetRectangleSize() const {
  float height = size.y - size.x;
  return glm::vec2(size.x, height);
}

void Capsule::Move(glm::vec2 offset, glm::vec4 boundaries) {
  glm::vec2 topLeft = GetPosition();
  glm::vec2 bottomRight = GetPosition() + GetSize();
  if (offset.x < 0 && topLeft.x + offset.x <= boundaries.x) {
    offset.x = boundaries.x - topLeft.x;
  }
  if (offset.x > 0 && bottomRight.x + offset.x >= boundaries.z) {
    offset.x = boundaries.z - bottomRight.x;
  }
  if (offset.y < 0 && topLeft.y + offset.y <= boundaries.y) {
    offset.y = boundaries.y - topLeft.y;
  }
  if (offset.y > 0 && bottomRight.y + offset.y >= boundaries.w) {
    offset.y = boundaries.w - bottomRight.y;
  }

  SetCenter(center + offset);
}

void Capsule::SetRectangleVisible(bool visible) {
  this->rectangleVisible = visible;
}

void Capsule::SetTopSemiCircleVisible(bool visible) {
  this->topSemiCircleVisible = visible;
}

void Capsule::SetBottomSemiCircleVisible(bool visible) {
  this->bottomSemiCircleVisible = visible;
}

bool Capsule::IsRectangleVisible() const { return rectangleVisible; }

bool Capsule::IsTopSemiCircleVisible() const { return topSemiCircleVisible; }

bool Capsule::IsBottomSemiCircleVisible() const {
  return bottomSemiCircleVisible;
}

bool Capsule::IsPositionInside(glm::vec2 mousePosition) {
  // If there's rotation for the capsule, the mouse position should be rotated
  // back to the original position.
  if (this->roll != 0.0f) {
    mousePosition = rotateVector(mousePosition, -this->roll, this->GetCenter());
  }
  // Get the radius of the semi-circle of the capsule
  float semiCircleRadius = GetRadius();
  // Get the center of the top semi-circle of the capsule
  glm::vec2 topSemiCircleCenter = GetTopSemiCircleCenter();
  // Get the center of the bottom semi-circle of the capsule
  glm::vec2 bottomSemiCircleCenter = GetBottomSemiCircleCenter();
  // Get the top left corner of the rectangle of the capsule
  glm::vec2 rectangleTopLeft = GetRectangleTopLeft();
  // Get the size of the rectangle of the capsule
  glm::vec2 rectangleSize = GetRectangleSize();
  // The mouse cursor should be in the rectangle or the semi-circles of the
  // capsule.
  bool isInRectangle = mousePosition.x > rectangleTopLeft.x &&
                       mousePosition.x < rectangleTopLeft.x + rectangleSize.x &&
                       mousePosition.y > rectangleTopLeft.y &&
                       mousePosition.y < rectangleTopLeft.y + rectangleSize.y;
  bool isInTopSemiCircle =
      glm::distance(glm::vec2(mousePosition.x, mousePosition.y),
                    topSemiCircleCenter) < semiCircleRadius;
  bool isInBottomSemiCircle =
      glm::distance(glm::vec2(mousePosition.x, mousePosition.y),
                    bottomSemiCircleCenter) < semiCircleRadius;

  return isInRectangle && rectangleVisible ||
         isInTopSemiCircle && topSemiCircleVisible ||
         isInBottomSemiCircle && bottomSemiCircleVisible;
}

void Capsule::Draw(std::shared_ptr<ColorRenderer> colorRenderer,
                   std::shared_ptr<CircleRenderer> circleRenderer) {
  // Draw the rectangle if it is visible
  if (rectangleVisible)
    colorRenderer->DrawColor(GetRectangleTopLeft(), GetRectangleSize(),
                             this->roll, this->rotationPivot, this->color);

  // Draw the top semicircle if it is visible
  if (topSemiCircleVisible)
    circleRenderer->DrawCircle(
        rotateVector(GetTopSemiCircleCenter(), this->roll, this->GetCenter()),
        GetRadius(), glm::pi<float>() + this->roll, this->color);

  // Draw the bottom semicircle if it is visible
  if (bottomSemiCircleVisible)
    circleRenderer->DrawCircle(rotateVector(GetBottomSemiCircleCenter(),
                                            this->roll, this->GetCenter()),
                               GetRadius(), 0.f + this->roll, this->color);
}

void Capsule::DrawOutline(std::shared_ptr<ColorRenderer> colorRenderer,
                          std::shared_ptr<CircleRenderer> circleRenderer) {
  // Draw the rectangle if it is visible
  if (rectangleVisible)
    colorRenderer->DrawEdge(GetRectangleTopLeft(), GetRectangleSize(),
                            this->roll, this->rotationPivot, this->color,
                            /*hideVerticalEdge=*/false,
                            /*hideHorizontalEdge=*/true);

  // Draw the top semicircle if it is visible
  if (topSemiCircleVisible)
    circleRenderer->DrawCircleEdge(
        rotateVector(GetTopSemiCircleCenter(), this->roll, this->GetCenter()),
        GetRadius(), glm::pi<float>() + this->roll, this->color);

  // Draw the bottom semicircle if it is visible
  if (bottomSemiCircleVisible)
    circleRenderer->DrawCircleEdge(rotateVector(GetBottomSemiCircleCenter(),
                                                this->roll, this->GetCenter()),
                                   GetRadius(), 0.f + this->roll, this->color);
}