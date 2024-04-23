#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>
#include "Bubble.h"
#include "RayRenderer.h"

// Ray is a class that represents a ray in 2D space. It starts from the bubble shooter and goes to the direction that the shooter is pointing at.
// It would bounce off the walls and end at the first static bubble that it hits.
// As it bounces off the walls, it would generate a list of points that represent the path that it takes.
// The path is used to draw the ray.
class Ray {
public:
	// Constructs a new Ray instance.
	Ray(glm::vec2 start, glm::vec2 dir, glm::vec4 color);
	Ray(glm::vec2 start, glm::vec2 dir);
	Ray();
	~Ray();

	// Getters and setters
	glm::vec2 GetDirection() const;
	glm::vec2 GetPosition() const;
	std::vector<glm::vec2> GetPath() const;
	glm::vec4 GetColor() const;
	glm::vec3 GetColorWithoutAlpha() const;
	void SetDirection(glm::vec2 dir);
	void SetPosition(glm::vec2 pos);
	void SetColor(glm::vec4 rgb);

	// Update the ray's path.
	void UpdatePath(glm::vec4 boundaries, std::unordered_map<int, std::unique_ptr<Bubble> >& statics);

	// Draw the ray.
	void Draw(std::shared_ptr<RayRenderer> RayRenderer);

private:
	glm::vec2 start, direction;
	std::vector<glm::vec2> path;
	glm::vec4 color;

	// Get the point of collision between the ray and the wall.
	glm::vec2 GetWallCollisionPoint(glm::vec2 pos, glm::vec2 dir, glm::vec4 boundaries);

	// Get the point of collision between the ray and the bubble.
	std::vector<glm::vec2> GetBubbleCollisionPoint(glm::vec2 pos, glm::vec2 dir, std::unordered_map<int, std::unique_ptr<Bubble> >& statics);

};