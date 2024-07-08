#include "Arrow.h"

Arrow::Arrow(glm::vec2 pos, glm::vec2 size, Texture2D sprite)
    : GameObject(pos, size, 0.0f, glm::vec2(0.5f, 0.5f), glm::vec2(0.0f, 0.0f),
                 glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), sprite),
      isFired(false),
      isStopped(false),
      isPenetrating(false),
      speed(0.0f),
      penetrationRatio(0.0f),
      penetrationPosition(glm::vec2(0.f)),
      targetPosition(glm::vec2(0.f)) {}

Arrow::Arrow(const Arrow& other)
    : GameObject(other),
      isFired(other.isFired),
      isStopped(other.isStopped),
      isPenetrating(other.isPenetrating),
      speed(other.speed),
      penetrationRatio(other.penetrationRatio),
      penetrationPosition(other.penetrationPosition),
      targetPosition(other.targetPosition) {}

Arrow::Arrow(Arrow&& other)
    : GameObject(other),
      isFired(other.isFired),
      isStopped(other.isStopped),
      isPenetrating(other.isPenetrating),
      speed(other.speed),
      penetrationRatio(other.penetrationRatio),
      penetrationPosition(other.penetrationPosition),
      targetPosition(other.targetPosition) {}

Arrow& Arrow::operator=(const Arrow& other) {
  if (this != &other) {
    GameObject::operator=(other);
    this->isFired = other.isFired;
    this->isStopped = other.isStopped;
    this->isPenetrating = other.isPenetrating;
    this->speed = other.speed;
    this->penetrationRatio = other.penetrationRatio;
    this->penetrationPosition = other.penetrationPosition;
    this->targetPosition = other.targetPosition;
  }
  return *this;
}

Arrow& Arrow::operator=(Arrow&& other) {
  if (this != &other) {
    GameObject::operator=(other);
    this->isFired = other.isFired;
    this->isStopped = other.isStopped;
    this->isPenetrating = other.isPenetrating;
    this->speed = other.speed;
    this->penetrationRatio = other.penetrationRatio;
    this->penetrationPosition = other.penetrationPosition;
    this->targetPosition = other.targetPosition;
  }
  return *this;
}

Arrow::~Arrow() {}

void Arrow::Fire(glm::vec2 target, float speed) {
  assert(!this->isFired && "Arrow is already fired");
  this->targetPosition = target;
  this->speed = speed;
  this->isFired = true;

  // Get a random penetration ratio.
  this->penetrationRatio = generateRandom(0.1f, 0.4f);
  // Set the penetration position to the target position
  this->penetrationPosition = this->targetPosition;
  // Recalculate the target position based on the penetration ratio
  this->targetPosition = this->penetrationPosition + GetArrowHeadDirection() *
                                                         this->size.x *
                                                         this->penetrationRatio;
  // Play the sound effect
  SoundEngine& soundEngine = SoundEngine::GetInstance();
  soundEngine.PlaySound("arrow_shoot");
  /*this->penetrationPosition = this->targetPosition - GetArrowHeadDirection() *
   * this->size.x * this->penetrationRatio;*/
}

glm::vec2 Arrow::GetArrowHeadDirection() {
  glm::vec2 direction = glm::vec2(1.0f, 0.0f);
  // Get target direction's radius based on the roll
  float radians = this->roll - glm::pi<float>();
  // Rotation the direction vector based on the roll
  glm::vec2 rotatedDirection = glm::rotate(direction, radians);
  return rotatedDirection;
}

void Arrow::Update(float dt) {
  if (this->isStopped) {
    return;
  }
  if (this->isFired) {
    // Calculate the penetration position based on the penetration ratio
    glm::vec2 direction = glm::normalize(this->targetPosition - this->position);
    this->position += direction * this->speed * dt;
    // Get the vector from the target to the arrow
    glm::vec2 toTarget = this->targetPosition - this->position;
    // Get the inner product of the direction and the vector to the target
    float innerProduct = glm::dot(direction, toTarget);
    // If the inner product is negative or equal to zero, the arrow has passed
    // the target, then reset the position of the arrow to the target and set
    // the hit status to true.
    if (innerProduct <= 0) {
      this->position = this->targetPosition;
      this->isStopped = true;
      SoundEngine& soundEngine = SoundEngine::GetInstance();
      soundEngine.PlaySound("arrow_hit");
      this->isPenetrating = false;
      return;
    }

    if (!this->isPenetrating) {
      //// Calculate the penetration position based on the penetration ratio
      // glm::vec2 penetrationPosition = this->targetPosition -
      // GetArrowHeadDirection() * this->size.x * this->penetrationRatio;
      //  Get distance from the penetration position to the target
      float distanceFromPenetrationToTarget =
          glm::length(this->targetPosition - this->penetrationPosition);
      // Get distance from the arrow to the target
      float distanceFromArrowToTarget =
          glm::length(this->targetPosition - this->position);
      // If the distance from the arrow to the target is less than or equal to
      // the distance from the penetration position to the target, then we set
      // the status of the arrow to penetrating.
      if (distanceFromArrowToTarget <= distanceFromPenetrationToTarget) {
        this->isPenetrating = true;
      }
    }
  }
}

bool Arrow::IsFired() { return this->isFired; }

bool Arrow::IsStopped() { return this->isStopped; }

bool Arrow::IsPenetrating() { return this->isPenetrating; }

glm::vec2 Arrow::GetPenetrationPosition() { return this->penetrationPosition; }

glm::vec4 Arrow::GetTextureCoords() {
  if (!this->isPenetrating && !this->isStopped) {
    return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
  } else {
    float actualPenetrationRatio =
        glm::distance(this->position, this->penetrationPosition) / this->size.x;
    return glm::vec4(actualPenetrationRatio, 0.0f, 1.0f, 1.0f);
  }
}

// void Arrow::Draw(std::shared_ptr<Renderer> renderer) {
//
//	if (this->isPenetrating) {
//		// Enable scissor test
//		glEnable(GL_SCISSOR_TEST);
//
//		// Get direction from the penetration position to the target
//		glm::vec2 direction = glm::normalize(this->targetPosition -
// this->penetrationPosition);
//
//
//		// Calculate the scissor box
//		glm::vec4 newScissorBox = glm::vec4(0.0f, 0.0f, (this->size.x +
// this->size.y) * this->scale, (this->size.x + this->size.y) * this->scale);
// if (direction.x < 0.f) { 			newScissorBox.x =
//this->penetrationPosition.x;
//		}
//		else if (direction.x > 0.f) {
//			newScissorBox.x = this->penetrationPosition.x -
// GetArrowHeadDirection().x * this->size.x * (1 - this->penetrationPosition.x);
//			newScissorBox.z = GetArrowHeadDirection().x *
// this->size.x * (1 - this->penetrationPosition.x);
//		}
//		else {
//			newScissorBox.x = 0.f;
//			newScissorBox.z = kWindowSize.x;
//		}
//		if (direction.y == 0.f) {
//			newScissorBox.y = 0.f;
//			newScissorBox.w = kWindowSize.y;
//		}
//		else if (direction.y > 0.f) {
//			newScissorBox.y = this->penetrationPosition.y;
//		}
//		else if (direction.y < 0.f) {
//			newScissorBox.y = kWindowSize.y -
//(this->penetrationPosition.y - GetArrowHeadDirection().y * this->size.y * (1 -
// this->penetrationPosition.y)); 			newScissorBox.w =
// GetArrowHeadDirection().y * this->size.y * (1 - this->penetrationPosition.y);
//		}
//
//		glScissor(newScissorBox.x, newScissorBox.y, newScissorBox.z,
// newScissorBox.w);
//	}
//
//	GameObject::Draw(renderer);
//
//	if (this->isPenetrating) {
//		// Disable scissor test
//		glDisable(GL_SCISSOR_TEST);
//	}
// }
