/*
 * Ray.cpp
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

#include "Ray.h"

Ray::Ray(glm::vec2 start, glm::vec2 dir, glm::vec4 color)
    : start(start), direction(dir), color(color) {}

Ray::Ray(glm::vec2 start, glm::vec2 dir) : start(start), direction(dir) {}

void Ray::UpdatePath(
    glm::vec4 boundaries,
    std::unordered_map<int, std::unique_ptr<Bubble> >& statics) {
  path.clear();
  path.emplace_back(start);
  glm::vec2 currentPos = start;
  glm::vec2 currentDir = direction;

  // Iterate through the path of the ray until it hits a static bubble or the
  // the upper wall.
  std::vector<glm::vec2> collisionWithBubble =
      GetBubbleCollisionPoint(currentPos, currentDir, statics);
  glm::vec2 collisionWithWall;
  int count = 0;
  while (collisionWithBubble.empty()) {
    collisionWithWall =
        GetWallCollisionPoint(currentPos, currentDir, boundaries);
    path.emplace_back(collisionWithWall);
    if (++count == 6) {
      return;
    }
    // If the ray hits the upper wall, then the path ends.
    if (areFloatsEqual(collisionWithWall.y, boundaries.y)) {
      return;
    }
    // Get the new direction of the ray after it bounces off the wall.
    glm::vec2 offsetPoint = collisionWithWall;
    if (areFloatsEqual(collisionWithWall.x, boundaries.x) ||
        areFloatsEqual(collisionWithWall.x, boundaries.z)) {
      // When the bubble (treated as a ray for simulation) collides with a wall,
      // it doesn't follow the exact reflection rule of a ray due to its area
      // size. To more accurately depict this interaction, an offset (yOffset)
      // is applied based on the bubble's radius and the direction of movement.
      float yOffset =
          -2.f * kBubbleRadius / std::abs(currentDir.x) * currentDir.y;
      offsetPoint =
          glm::vec2(collisionWithWall.x, collisionWithWall.y + yOffset);
      path.emplace_back(offsetPoint);
      currentDir.x = -currentDir.x;
    }
    if (areFloatsEqual(collisionWithWall.y, boundaries.w)) {
      // To more accurately depict this interaction, an offset (xOffset)
      //  is applied based on the bubble's radius and the direction of movement.
      float xOffset =
          -2.f * kBubbleRadius / std::abs(currentDir.y) * currentDir.x;
      offsetPoint =
          glm::vec2(collisionWithWall.x + xOffset, collisionWithWall.y);
      path.emplace_back(offsetPoint);
      currentDir.y = -currentDir.y;
    }
    currentPos = offsetPoint;
    collisionWithBubble =
        GetBubbleCollisionPoint(currentPos, currentDir, statics);
  }
  // If the ray hits a static bubble, then the path ends.
  if (collisionWithBubble.empty()) {
    return;
  }
  path.emplace_back(collisionWithBubble[0]);
}

glm::vec2 Ray::GetWallCollisionPoint(glm::vec2 pos, glm::vec2 dir,
                                     glm::vec4 boundaries) {
  // Calculate the paramters of the line equation. Ax + By + C = 0
  float A = dir.y;
  // If A == 0, then the line is horizontal. The line will only possible to
  // collide with the left or right boundary.
  if (areFloatsEqual(A, 0.f)) {
    return dir.x > 0 ? glm::vec2(boundaries.z, pos.y)
                     : glm::vec2(boundaries.x, pos.y);
  }
  float B = -dir.x;
  // If B == 0, then the line is vertical. The line will only possible to
  // collide with the upper and lower boundary.
  if (areFloatsEqual(B, 0.f)) {
    return dir.y > 0 ? glm::vec2(pos.x, boundaries.w)
                     : glm::vec2(pos.x, boundaries.y);
  }
  float C = -A * pos.x - B * pos.y;
  // Calculate the intersection point between the line and the left boundary.
  float y = -(C + A * boundaries.x) / B;
  if (dir.x < 0 && y >= boundaries.y && y <= boundaries.w) {
    return glm::vec2(boundaries.x, y);
  }

  // Calculate the intersection point between the line and the right boundary.
  y = -(C + A * boundaries.z) / B;
  if (dir.x > 0 && y >= boundaries.y && y <= boundaries.w) {
    return glm::vec2(boundaries.z, y);
  }

  // Calculate the intersection point between the line and the upper boundary.
  float x = -(C + B * boundaries.y) / A;
  if (dir.y < 0 && x >= boundaries.x && x <= boundaries.z) {
    return glm::vec2(x, boundaries.y);
  }

  // Calculate the intersection point between the line and the lower boundary.
  x = -(C + B * boundaries.w) / A;
  assert(dir.y > 0 && x >= boundaries.x && x <= boundaries.z &&
         "The ray must cross one of the boundaries.");

  return glm::vec2(x, boundaries.w);
}

std::vector<glm::vec2> Ray::GetBubbleCollisionPoint(
    glm::vec2 pos, glm::vec2 dir,
    std::unordered_map<int, std::unique_ptr<Bubble> >& statics) {
  // Calculate the paramters of the line equation. Ax + By + C = 0
  float A = dir.y;
  float B = -dir.x;
  float C = -A * pos.x - B * pos.y;

  // Iterate through all the static bubbles to find the closest collision point.
  // a is the distance between the line and the center of the bubble.
  // b is the distance between the middle of the intersection points and the
  // start position of the ray. c is the distance between the center of the
  // bubble and the start position of the ray.

  float a = std::numeric_limits<float>::max();
  float b = std::numeric_limits<float>::max();

  int collisionBubbleID = -1;
  for (auto& bubble : statics) {
    // vector from the start position of the ray to the center of the bubble.
    glm::vec2 PO = bubble.second->GetCenter() - pos;
    if (glm::dot(PO, dir) < 0.f) {
      continue;
    }

    // Calculate the distance between the line and the bubble.
    float curA = std::abs(A * bubble.second->GetCenter().x +
                          B * bubble.second->GetCenter().y + C) /
                 std::sqrt(A * A + B * B);
    if (curA > bubble.second->GetRadius()) {
      continue;
    }

    // Get the distance between the center of the bubble to the start position
    // of the ray.
    float curC = glm::distance(bubble.second->GetCenter(), pos);
    float curB = std::sqrt(curC * curC - curA * curA);
    if (curB < b) {
      b = curB;
      a = curA;
      collisionBubbleID = bubble.first;
    }
  }
  if (collisionBubbleID == -1) {
    return std::vector<glm::vec2>();
  }

  // Get distance between the intersection point (G, H) and the middle of the
  // intersection points (M).
  float distGM = std::sqrt(statics[collisionBubbleID]->GetRadius() *
                               statics[collisionBubbleID]->GetRadius() -
                           a * a);

  // Get the distance between the start position of the ray (P) and the
  // intersection points (G, H).
  float distPG = b - distGM;
  float distPH = b + distGM;

  // Get the intersection points (G, H).
  glm::vec2 G = pos + distPG * glm::normalize(dir);
  glm::vec2 H = pos + distPH * glm::normalize(dir);

  return std::vector<glm::vec2>{G, H};
  //// Get the vector from the start position of the ray to the middle of the
  /// intersection points.
  // glm::vec2 PM = b * glm::normalize(dir);
  //// Get the vector from the center of the bubble to the start position of the
  /// ray.
  // glm::vec2 OP = pos- statics[collisionBubbleID]->GetCenter();
  //// Get the vector from the center of the bubble to the middle of the
  /// intersection points.
  // glm::vec2 OM = OP + PM;
  //// Normalize the vector from the center of the bubble to the middle of the
  /// intersection points.
  // OM = glm::normalize(OM);
  //// Get the point that is on the circle and on the line crossing the center
  /// of the bubble and the middle of the intersection points.
  // glm::vec2 perpendicular = statics[collisionBubbleID]->GetCenter() +
  // statics[collisionBubbleID]->GetRadius() * OM;

  //// Get the cosines of the angle between the distance line and the line that
  /// connects the center of the bubble and the collision point.
  // float cosAngle = a / statics[collisionBubbleID]->GetRadius();
  //// Calculate the the angle from cosAngle.
  // float angle = glm::acos(cosAngle);

  //// Calculate the intersection points between the line and the bubble by
  /// rotating the perpendicular vector by angle and -angle.
  // glm::vec2 bubbleCenter = statics[collisionBubbleID]->GetCenter();
  // glm::vec2 intersection1 = rotateVector(perpendicular, angle, bubbleCenter);
  // glm::vec2 intersection2 = rotateVector(perpendicular, -angle,
  // bubbleCenter); std::vector<glm::vec2> result{ intersection1, intersection2
  // };
  //// Switch the intersection points if the start position of the ray is closer
  /// to the second intersection point.
  // if (glm::distance(pos, intersection2) < glm::distance(pos, intersection1))
  // { 	std::swap(result[0], result[1]);
  // }
  // return result;
}

void Ray::Draw(std::shared_ptr<RayRenderer> rayRenderer) {
  rayRenderer->DrawRay(path, color);
}

glm::vec2 Ray::GetDirection() const { return direction; }

glm::vec2 Ray::GetPosition() const { return start; }

std::vector<glm::vec2> Ray::GetPath() const { return path; }

glm::vec4 Ray::GetColor() const { return color; }

glm::vec3 Ray::GetColorWithoutAlpha() const {
  return glm::vec3(color.r, color.g, color.b);
}

void Ray::SetDirection(glm::vec2 dir) { direction = dir; }

void Ray::SetPosition(glm::vec2 pos) { start = pos; }

void Ray::SetColor(glm::vec4 col) { color = col; }
