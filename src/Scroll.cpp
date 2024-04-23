#include "Scroll.h"

const float Scroll::kSilkEdgeWidth = 0.1f*kBubbleRadius;

Scroll::Scroll(glm::vec2 center, glm::vec2 rollerSize, glm::vec2 silkSize, Texture2D rollerTexture) : GameObject(), center(center), rollerSize(rollerSize), silkSize(silkSize), state(ScrollState::OPENED), targetSilkLenForNarrowing(0.f), currentSilkLenForNarrowing(0.f), targetSilkLenForClosing(0.f), targetSilkLenForOpening(0.f),
velocityForNarrowing(kBubbleRadius), velocityForClosing(48 * kBubbleRadius), velocityForOpening(96 * kBubbleRadius), velocityForRetracting(24 * kBubbleRadius),
targetPositionForRetracting(glm::vec2(0.f)), targetPositionForDeploying(glm::vec2(0.f)), targetPositionForAttacking(glm::vec2(0.f)){
	float silkLen = silkSize.y;
	topRoller = std::make_unique<GameObject>(/*pos=*/center - glm::vec2(rollerSize.x / 2.0f, silkLen / 2.0f + rollerSize.y * 0.85), /*size=*/rollerSize, /*roll=*/0.f, /*rotationPivot=*/glm::vec2(0.5f, 0.5f), /*velocity=*/glm::vec2(0.0f, 0.0f), /*color=*/glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), /*sprite=*/rollerTexture);
	bottomRoller = std::make_unique<GameObject>(/*pos=*/center + glm::vec2(-rollerSize.x / 2.0f, silkLen / 2.0f - rollerSize.y * 0.155), /*size=*/rollerSize, /*roll=*/0.f, /*rotationPivot=*/glm::vec2(0.5f, 0.5f), /*velocity=*/glm::vec2(0.0f, 0.0f), /*color=*/glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), /*sprite=*/rollerTexture);
	
	glm::vec2 rotationPivotForTopRoller = (center - topRoller->GetPosition()) / topRoller->GetSize();
	glm::vec2 rotationPivotForBottomRoller = (center - bottomRoller->GetPosition()) / bottomRoller->GetSize();
	topRoller->SetRotationPivot(rotationPivotForTopRoller);
	bottomRoller->SetRotationPivot(rotationPivotForBottomRoller);

	//// Update the postion of the whole scroll, It equal the position of the top roller.
	//SetPosition(topRoller->GetPosition());
}

Scroll::~Scroll() {}

glm::vec2 Scroll::GetPosition() const {
	return topRoller->GetPosition();
}

glm::vec2 Scroll::GetCenter() const {
	return center;
}

glm::vec2 Scroll::GetRollerSize() const {
	return rollerSize;
}

float Scroll::GetSilkLen() const {
	return silkSize.y;
}

float Scroll::GetSilkWidth() const {
	return silkSize.x;
}

glm::vec4 Scroll::GetSilkBounds() const {
	return glm::vec4(center.x - silkSize.x / 2.0f, center.y - silkSize.y / 2.0f, center.x + silkSize.x / 2.0f, center.y + silkSize.y / 2.0f);
}

std::unique_ptr<GameObject>& Scroll::GetTopRoller() {
	return topRoller;
}

std::unique_ptr<GameObject>& Scroll::GetBottomRoller() {
	return bottomRoller;
}

void Scroll::SetSilkLen(float silkLen) {
	this->silkSize.y = silkLen;
	topRoller->SetPosition(center - glm::vec2(rollerSize.x / 2.0f, silkLen / 2.0f + rollerSize.y * 0.85));
	bottomRoller->SetPosition(center + glm::vec2(-rollerSize.x / 2.0f, silkLen / 2.0f - rollerSize.y * 0.155));
	if (silkLen == 0.f) {
		std::cout << "center: " << center.x << ", " << center.y << std::endl;
		std::cout << "toproller position: " << topRoller->GetPosition().x << ", " << topRoller->GetPosition().y << std::endl;
		std::cout << "bottomroller position: " << bottomRoller->GetPosition().x << ", " << bottomRoller->GetPosition().y << std::endl;
	}
	glm::vec2 rotationPivotForTopRoller = (center - topRoller->GetPosition()) / topRoller->GetSize();
	glm::vec2 rotationPivotForBottomRoller = (center - bottomRoller->GetPosition()) / bottomRoller->GetSize();
	topRoller->SetRotationPivot(rotationPivotForTopRoller);
	bottomRoller->SetRotationPivot(rotationPivotForBottomRoller);

	// Update the postion of the whole scroll, It equal the position of the top roller.
	this->position = topRoller->GetPosition();
}

void Scroll::SetPosition(glm::vec2 pos) {
	// this function is not allowed to be called.
	assert(false);
}

void Scroll::AddSilkLen(float delta) {
	this->silkSize.y += delta;
	topRoller->SetPosition(topRoller->GetPosition() - glm::vec2(0, delta*0.5));
	bottomRoller->SetPosition(bottomRoller->GetPosition() + glm::vec2(0, delta*0.5));

	// Update the rotation pivot of the rollers.
	glm::vec2 rotationPivotForTopRoller = (center - topRoller->GetPosition()) / topRoller->GetSize();
	glm::vec2 rotationPivotForBottomRoller = (center - bottomRoller->GetPosition()) / bottomRoller->GetSize();
	topRoller->SetRotationPivot(rotationPivotForTopRoller);
	bottomRoller->SetRotationPivot(rotationPivotForBottomRoller);
}

void Scroll::SetCenter(glm::vec2 center) {
	glm::vec2 translation = center - this->center;
	topRoller->SetPosition(topRoller->GetPosition() + translation);
	bottomRoller->SetPosition(bottomRoller->GetPosition() + translation);

	glm::vec2 rotationPivotForTopRoller = (center - topRoller->GetPosition()) / topRoller->GetSize();
	glm::vec2 rotationPivotForBottomRoller = (center - bottomRoller->GetPosition()) / bottomRoller->GetSize();
	topRoller->SetRotationPivot(rotationPivotForTopRoller);
	bottomRoller->SetRotationPivot(rotationPivotForBottomRoller);

	// Update the center of the scroll.
	this->center = center;
}

void Scroll::SetTargetCenter(std::string target, glm::vec2 center) {
	this->targetCenters[target] = center;
}

glm::vec2 Scroll::GetTargetCenter(std::string target) const{
	assert(this->targetCenters.count(target) > 0 && "The target center is not set.");
	return this->targetCenters.at(target);
}

void Scroll::SetState(ScrollState state) {
	this->state = state;
}

ScrollState Scroll::GetState() const {
	return state;
}

void Scroll::SetRoll(float roll) {
	this->roll = roll;
	topRoller->SetRoll(roll);
	bottomRoller->SetRoll(roll);
}

void Scroll::SetScale(float scale) {
	this->scale = scale;
	topRoller->SetScale(scale);
	bottomRoller->SetScale(scale);
}

void Scroll::SetAlpha(float alpha) {
	this->color.a = alpha;
	topRoller->SetColor(glm::vec4(topRoller->GetColor().r, topRoller->GetColor().g, topRoller->GetColor().b, alpha));
	bottomRoller->SetColor(glm::vec4(bottomRoller->GetColor().r, bottomRoller->GetColor().g, bottomRoller->GetColor().b, alpha));
}

void Scroll::SetTargetSilkLenForNarrowing(float targetSilkLen) {
	this->targetSilkLenForNarrowing = targetSilkLen;
}

void Scroll::SetCurrentSilkLenForNarrowing(float currentSilkLen) {
	this->currentSilkLenForNarrowing = currentSilkLen;
}

void Scroll::SetTargetSilkLenForClosing(float targetSilkLen) {
	this->targetSilkLenForClosing = targetSilkLen;
}

void Scroll::SetTargetSilkLenForOpening(float targetSilkLen) {
	this->targetSilkLenForOpening = targetSilkLen;
}

float Scroll::GetTargetSilkLenForNarrowing() const {
	return targetSilkLenForNarrowing;
}

float Scroll::GetCurrentSilkLenForNarrowing() const {
	return currentSilkLenForNarrowing;
}

float Scroll::GetTargetSilkLenForClosing() const {
	return targetSilkLenForClosing;
}

float Scroll::GetTargetSilkLenForOpening() const {
	return targetSilkLenForOpening;
}

void Scroll::SetVelocityForNarrowing(float velocity) {
	this->velocityForNarrowing = velocity;
}

void Scroll::SetVelocityForClosing(float velocity) {
	this->velocityForClosing = velocity;
}

void Scroll::SetVelocityForOpening(float velocity) {
	this->velocityForOpening = velocity;
}

float Scroll::GetVelocityForNarrowing() const {
	return velocityForNarrowing;
}

float Scroll::GetVelocityForClosing() const {
	return velocityForClosing;
}

float Scroll::GetVelocityForOpening() const {
	return velocityForOpening;
}

void Scroll::SetTargetPositionForRetracting(glm::vec2 targetPosition) {
	this->targetPositionForRetracting = targetPosition;
}

void Scroll::SetTargetPositionForDeploying(glm::vec2 targetPosition) {
	this->targetPositionForDeploying = targetPosition;
}

void Scroll::SetTargetPositionForAttacking(glm::vec2 targetPosition) {
	this->targetPositionForAttacking = targetPosition;
}

glm::vec2 Scroll::GetTargetPositionForRetracting() const {
	return targetPositionForRetracting;
}

glm::vec2 Scroll::GetTargetPositionForDeploying() const {
	return targetPositionForDeploying;
}

glm::vec2 Scroll::GetTargetPositionForAttacking() const {
	return targetPositionForAttacking;
}

void Scroll::Move(float dt) {
	glm::vec2 distanceVector = calculateTravelDistanceVector(velocity, acceleration, dt);
	this->SetVelocity(calculateVelocity(velocity, acceleration, dt));
	this->SetCenter(center + distanceVector);
}

void Scroll::Narrow(float dt, float targetSilkLen) {
	assert(this->state == ScrollState::NARROWING);
	// narrow the scroll gradually
	float diff = this->GetSilkLen() - targetSilkLen;
	// narrowing distance based on the velocity
	float distance = velocityForNarrowing * dt;
	if (diff > 0.f) {
		if (diff <= distance) {
			this->AddSilkLen(-diff);
			this->currentSilkLenForNarrowing = targetSilkLen;
			this->SetState(ScrollState::NARROWED);
		}
		else {
			this->AddSilkLen(-distance);
			this->currentSilkLenForNarrowing = this->GetSilkLen();
		}
	}
}

void Scroll::Close(float dt, float targetSilkLen) {
	assert(this->state == ScrollState::CLOSING);
	// close the scroll gradually
	float diff = this->GetSilkLen() - targetSilkLen;
	// closing distance based on the velocity
	float distance = velocityForClosing * dt;
	if (diff > 0.f) {
		if (diff <= distance) {
			this->AddSilkLen(-diff);
			this->SetState(ScrollState::CLOSED);
		}
		else {
			this->AddSilkLen(-distance);
		}
	}
}

void Scroll::Open(float dt, float targetSilkLen) {
	assert(this->state == ScrollState::OPENING);
	// open the scroll gradually
	float diff = targetSilkLen - this->GetSilkLen();
	// opening distance based on the velocity
	float distance = velocityForOpening * dt;
	if (diff > 0.f) {
		if (diff <= distance) {
			this->AddSilkLen(diff);
			this->SetState(ScrollState::OPENED);
		}
		else {
			this->AddSilkLen(distance);
		}
	}
	else {
		this->SetState(ScrollState::OPENED);
	}
}

void Scroll::Retract(float dt) {
	assert(this->state == ScrollState::RETRACTING);
	glm::vec2 diffVector = targetPositionForRetracting - this->center;
	glm::vec2 dir = glm::normalize(diffVector);
	float distance = velocityForRetracting * dt;
	glm::vec2 newCenter = this->center + dir * distance;
	glm::vec2 newDiffVector = targetPositionForRetracting - newCenter;

	float innerProduct = glm::dot(diffVector, newDiffVector);
	if (innerProduct <= 0.f) {
		this->SetCenter(targetPositionForRetracting);
		// Set the the scroll to be invisible
		this->SetAlpha(0.f);
		this->SetState(ScrollState::RETRACTED);
	}
	else {
		this->SetCenter(newCenter);
		this->SetRoll(this->GetRoll() - 8*glm::pi<float>() * dt);
		float newScale = this->GetScale() - 0.9 * dt;
		if (newScale > 0.05f) {
			this->SetScale(newScale);
		}
		else {
			this->SetScale(0.05f);
		}
	}
}

void Scroll::Deploy(float dt) {
	assert(this->state == ScrollState::DEPLOYING);
	glm::vec2 diffVector = targetPositionForDeploying - this->center;
	glm::vec2 dir = glm::normalize(diffVector);
	float distance = velocityForRetracting * dt;
	glm::vec2 newCenter = this->center + dir * distance;
	glm::vec2 newDiffVector = targetPositionForDeploying - newCenter;

	float innerProduct = glm::dot(diffVector, newDiffVector);
	if (innerProduct <= 0.f) {
		this->SetCenter(targetPositionForDeploying);
		this->SetRoll(0.f);
		this->SetScale(1.f);
		this->SetState(ScrollState::DEPLOYED);
	}
	else {
		this->SetCenter(newCenter);
		this->SetRoll(this->GetRoll() + 8*glm::pi<float>() * dt);
		float newScale = this->GetScale() + 0.8 * dt;
		if (newScale < 1.f) {
			this->SetScale(newScale);
		}
		else {
			this->SetScale(1.f);
		}
	}
}

void Scroll::Attack(float dt) {
	assert(this->state == ScrollState::ATTACKING);
	glm::vec2 diffVector = targetPositionForAttacking - this->center;
	glm::vec2 dir = glm::normalize(diffVector);
	float distance = velocityForRetracting * dt;
	glm::vec2 newCenter = this->center + dir * distance;
	glm::vec2 newDiffVector = targetPositionForAttacking - newCenter;

	float innerProduct = glm::dot(diffVector, newDiffVector);
	if (innerProduct <= 0.f) {
		this->SetCenter(targetPositionForAttacking);
		this->SetRoll(-glm::pi<float>()/3.f);
		this->SetState(ScrollState::ATTACKED);
	}
	else {
		this->SetCenter(newCenter);
		this->SetRoll(this->GetRoll() + 8 * glm::pi<float>() * dt);
		float newScale = this->GetScale() - 0.9 * dt;
		if (newScale > 0.06f) {
			this->SetScale(newScale);
		}
		else {
			this->SetScale(0.06f);
		}
	}
}

void Scroll::Return(float dt) {
	assert(this->state == ScrollState::RETURNING);
	glm::vec2 diffVector = targetPositionForDeploying - this->center;
	glm::vec2 dir = glm::normalize(diffVector);
	float distance = velocityForRetracting * dt;
	glm::vec2 newCenter = this->center + dir * distance;
	glm::vec2 newDiffVector = targetPositionForDeploying - newCenter;

	float innerProduct = glm::dot(diffVector, newDiffVector);
	if (innerProduct <= 0.f) {
		this->SetCenter(targetPositionForDeploying);
		this->SetRoll(0.f);
		this->SetScale(1.f);
		this->SetState(ScrollState::RETURNED);
	}
	else {
		this->SetCenter(newCenter);
		this->SetRoll(this->GetRoll() - 8 * glm::pi<float>() * dt);
		float newScale = this->GetScale() + 0.8 * dt;
		if (newScale < 1.f) {
			this->SetScale(newScale);
		}
		else {
			this->SetScale(1.f);
		}
	}
}

void Scroll::Reset() {
	this->SetScale(1.f);
	this->SetAlpha(1.f);
	this->SetRoll(0.f);
	this->SetSilkLen(this->GetTargetSilkLenForClosing());
	this->SetCenter(this->GetTargetCenter("outofscreen"));
	this->SetVelocity(glm::vec2(0.f));
	this->SetState(ScrollState::CLOSED);
}

void Scroll::Draw(std::shared_ptr<Renderer> renderer) {
	topRoller->Draw(renderer);
	bottomRoller->Draw(renderer);
}