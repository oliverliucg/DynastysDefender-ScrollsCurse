#include "GameCharacter.h"

GameCharacter::GameCharacter(const std::string& name, GameCharacterState state, std::unordered_map<GameCharacterState, glm::vec2> positions, std::unordered_map<GameCharacterState, glm::vec2> sizes, float rotation, glm::vec2 pivot, std::unordered_map<GameCharacterState, Texture2D> textures) : GameObject(positions[state], sizes[state], rotation, pivot, glm::vec2(0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), textures[state]), name(name), state(state), positions(positions), sizes(sizes), textures(textures), targetPosition(glm::vec2(0.f)), targetRoll(0.f), faceLeft(true), isMoving(false), isRotating(false), isStunned(false) {
}

GameCharacter::GameCharacter(const GameCharacter& other) : GameObject(other) {
	name = other.name;
	state = other.state;
	health = other.health == nullptr? nullptr : std::make_unique<Health>(*other.health);
	positions = other.positions;
	sizes = other.sizes;
	textures = other.textures;
	carriedObjects = other.carriedObjects;
	targetPosition = other.targetPosition;
	targetRoll = other.targetRoll;
	faceLeft = other.faceLeft;
	isMoving = other.isMoving;
	isRotating = other.isRotating;
	isStunned = other.isStunned;
}

GameCharacter::GameCharacter(GameCharacter&& other) noexcept : GameObject(other) {
	name = other.name;
	state = other.state;
	health = std::move(other.health);
	positions = other.positions;
	sizes = other.sizes;
	textures = other.textures;
	carriedObjects = other.carriedObjects;
	targetPosition = other.targetPosition;
	targetRoll = other.targetRoll;
	faceLeft = other.faceLeft;
	isMoving = other.isMoving;
	isRotating = other.isRotating;
	isStunned = other.isStunned;
}

GameCharacter& GameCharacter::operator=(const GameCharacter& other) {
	GameObject::operator=(other);
	name = other.name;
	state = other.state;
	health = other.health == nullptr ? nullptr : std::make_unique<Health>(*other.health);
	positions = other.positions;
	sizes = other.sizes;
	textures = other.textures;
	carriedObjects = other.carriedObjects;
	targetPosition = other.targetPosition;
	targetRoll = other.targetRoll;
	faceLeft = other.faceLeft;
	isMoving = other.isMoving;
	isRotating = other.isRotating;
	isStunned = other.isStunned;
	return *this;
}

GameCharacter& GameCharacter::operator=(GameCharacter&& other) noexcept {
	GameObject::operator=(other);
	name = other.name;
	state = other.state;
	health = std::move(other.health);
	positions = other.positions;
	sizes = other.sizes;
	textures = other.textures;
	carriedObjects = other.carriedObjects;
	targetPosition = other.targetPosition;
	targetRoll = other.targetRoll;
	faceLeft = other.faceLeft;
	isMoving = other.isMoving;
	isRotating = other.isRotating;
	isStunned = other.isStunned;
	return *this;
}

GameCharacter::~GameCharacter() {
}

void GameCharacter::DrawGameCharacter(std::shared_ptr<SpriteRenderer> sprideRenderer, std::shared_ptr<ColorRenderer> colorRenderer, std::shared_ptr<CircleRenderer> circleRenderer, std::shared_ptr<TextRenderer> textRenderer) {
	GameObject::Draw(sprideRenderer);
	if (health != nullptr) {
		if (this->state == GameCharacterState::FIGHTING) {
			health->DrawHealthBar(colorRenderer, circleRenderer);
		}
		health->DrawDamageTexts(textRenderer);
	}
}

std::string GameCharacter::GetName() const {
	return name;
}

GameCharacterState GameCharacter::GetState() const {
	return state;
}


void GameCharacter::SetState(GameCharacterState state) {
	this->state = state;
	GameObject::SetPosition(positions[state]);
	/*SetPosition(positions[state]);*/
	/*SetPosition(positions[state]);*/
	SetSize(sizes[state]);
	SetTexture(textures[state]);
}

Health& GameCharacter::GetHealth() {
	if (health == nullptr) {
		health = std::make_unique<Health>(-1);
	}
	return *health;
}

void GameCharacter::SetAndMoveToTargetPosition(glm::vec2 position) {
	targetPosition = position;
	isMoving = position != GetPosition()? true : false;
}

void GameCharacter::SetTargetPosition(const std::string& targetName, glm::vec2 position) {
	this->targetPositions[targetName] = position;
}

glm::vec2 GameCharacter::GetTargetPosition(const std::string& targetName) const {
	assert(this->targetPositions.count(targetName) > 0 && "The target position is not set");
	return this->targetPositions.at(targetName);
}

void GameCharacter::SetTargetRoll(float roll) {
	targetRoll = roll;
	isRotating = roll != GetRoll()? true : false;
}

glm::vec2 GameCharacter::GetCurrentTargetPosition() const {
	return targetPosition;
}

float GameCharacter::GetTargetRoll() const {
	return targetRoll;
}

std::unordered_map<int, std::unordered_map<GameCharacterState, glm::vec2> >& GameCharacter::GetRelativeCenterRatios() {
	return relativeCenterRatios;
}

void GameCharacter::SetFaceDirection(bool faceLeft) {
	this->faceLeft = faceLeft;
}

std::unordered_map<int, std::shared_ptr<GameObject> >& GameCharacter::GetCarriedObjects() {
	return carriedObjects;
}

void GameCharacter::AddCarriedObject(std::shared_ptr<GameObject> object) {
	assert(object != nullptr && "The object to be carried cannot be null");
	assert(carriedObjects.find(object->GetID()) == carriedObjects.end() && "The object to be carried is already carried by the character");
	carriedObjects[object->GetID()] = object;

	// Set the rotation pivot of the carried object to the rotation pivot of the character
	glm::vec2 rotationPivotCoord = GetRotationPivotCoord();
	glm::vec2 rotationPivotForCarriedObject = (rotationPivotCoord - object->GetPosition())/object->GetSize();
	object->SetRotationPivot(rotationPivotForCarriedObject);

	// Set the relative center ratio of the carried object
	glm::vec2 objectCenter = object->GetPosition() + object->GetSize() * 0.5f;
	this->SetRelativeCenterRatio(this->state, object->GetID(), objectCenter);
}

void GameCharacter::ActivateStun() {
	isStunned = true;
}

void GameCharacter::DeactivateStun() {
	isStunned = false;
}

bool GameCharacter::IsMoving() const {
	return isMoving;
}

bool GameCharacter::IsRotating() const {
	return isRotating;
}

bool GameCharacter::IsStunned() const {
	return isStunned;
}

void GameCharacter::SetPosition(glm::vec2 position) {
	// Get the position based on the state
	glm::vec2 currentStatePos = positions[state];
	assert(areFloatsEqual(currentStatePos, this->position) && "The position of the character is not consistent with the state");
	// Get the offset
	glm::vec2 offset = position - this->position;
	// Update the position of the character
	this->position += offset;
	// Update the position of each state
	for (auto& p : positions) {
		positions[p.first] += offset;
	}
	UpdateCarriedObjectsPosition();
	UpdateHealthBarPosition();
}

void GameCharacter::SetAlpha(float alpha) {
	GameObject::SetAlpha(alpha);
	for (auto& object : carriedObjects) {
		object.second->SetAlpha(alpha);
	}
}

void GameCharacter::MoveTo(glm::vec2 position, float velocity, float dt) {
	if (!isMoving) {
		return;
	}
	glm::vec2 currentPos = GetPosition();
	glm::vec2 direction = position - currentPos;
	float distance = glm::length(direction);
	direction = glm::normalize(direction);
	glm::vec2 newPosition = currentPos + direction * velocity * dt;
	float rest = distance - velocity*dt;
	if (rest >= 0.f) {
		SetPosition(newPosition);
	}
	else {
		SetPosition(position);
		isMoving = false;
	}
	//// Set the position for the carried objects
	//UpdateCarriedObjectsPosition();
	//// Set the position for the health bar
	//UpdateHealthBarPosition();
	/*glm::vec2 offset = this->position - currentPos;*/
	//for (auto& object : carriedObjects) {
	//	object.second->SetPosition(object.second->GetPosition() + offset);
	//}
	//// Set the position for the health bar
	//if (this->health != nullptr) {
	//	this->GetHealth().GetTotalHealthBar().SetCenter(this->GetHealth().GetTotalHealthBar().GetCenter() + offset);
	//	this->GetHealth().GetCurrentHealthBar().SetCenter(this->GetHealth().GetCurrentHealthBar().GetCenter() + offset);
	//	std::cout << "new center: " << this->GetHealth().GetTotalHealthBar().GetCenter().x << ", " << this->GetHealth().GetTotalHealthBar().GetCenter().y << std::endl;
	//}
}

void GameCharacter::MoveTo(glm::vec2 position, float velocity, float acceleration, float dt) {
	float avgVelocity = velocity + acceleration * dt * 0.5f;
	this->MoveTo(position, avgVelocity, dt);
}

void GameCharacter::RotateTo(float angle, float angularVelocity, float dt) {
	if (!isRotating) {
		return;
	}
	float currentAngle = GetRoll();
	float rest = angle - currentAngle;
	if (rest == 0.f) {
		isRotating = false;
		return;
	}
	// rest and the angular velocity have the same sign, if not, return true
	assert((rest > 0.f && angularVelocity > 0.f) || (rest < 0.f && angularVelocity < 0.f));

	float deltaAngle = angularVelocity * dt;
	float newAngle = currentAngle + deltaAngle;
	if (rest > 0.f && newAngle > angle || rest < 0.f && newAngle < angle) {
		newAngle = angle;
		deltaAngle = angle - currentAngle;
		isRotating = false;
	}
	SetRoll(newAngle);

	// Set roll for the carried objects
	for (auto& object : carriedObjects) {
		object.second->SetRoll(object.second->GetRoll() + deltaAngle);
	}
}

void GameCharacter::SetRelativeCenterRatio(GameCharacterState state, int id, glm::vec2 center) {
	// Get character's center
	glm::vec2 characterCenter = positions[state] + sizes[state] * 0.5f;
	// Get the relative center ratio
	glm::vec2 relativeCenterRatio = (center - characterCenter) / GetSize();
	relativeCenterRatios[id][state] = relativeCenterRatio;
}

void GameCharacter::SetHealthBarRelativeCenterRatios() {
	// Get character's center
	glm::vec2 characterCenter = GetPosition() + GetSize() * 0.5f;
	// Get total health bar's center
	glm::vec2 totalHealthBarCenter = this->health->GetTotalHealthBar().GetCenter();
	// Get current health bar's center
	glm::vec2 currentHealthBarCenter = this->health->GetCurrentHealthBar().GetCenter();
	// Set the relative center ratio for the total health bar
	glm::vec2 relativeCenterRatio = (totalHealthBarCenter - characterCenter) / GetSize();
	relativeCenterRatios[this->health->GetTotalHealthBar().GetID()][GameCharacterState::FIGHTING] = relativeCenterRatio;
	// Set the relative center ratio for the current health bar
	relativeCenterRatio = (currentHealthBarCenter - characterCenter) / GetSize();
	relativeCenterRatios[this->health->GetCurrentHealthBar().GetID()][GameCharacterState::FIGHTING] = relativeCenterRatio;
}

void GameCharacter::UpdateCarriedObjectsPosition() {
	std::unordered_map<GameCharacterState, glm::vec2> characterCenters;
	for(auto& p : positions) {
		if (p.first == this->state) {
			assert(areFloatsEqual(this->position, p.second) && "The position of the character is not consistent with the state");
		}
		characterCenters[p.first] = p.second + sizes[p.first] * 0.5f;
	}
	for (auto& object : carriedObjects) {
		assert(relativeCenterRatios.count(object.first) > 0 && "The relative center ratio for the carried object is not set");
		for(auto& [state, relativeCenterRatio] : relativeCenterRatios[object.first]) {
			glm::vec2 objectCenter = characterCenters[state] + relativeCenterRatio * sizes[state];
			object.second->SetCenter(objectCenter);
		}
	}
}

void GameCharacter::UpdateHealthBarPosition() {
	if (this->health != nullptr) {
		glm::vec2 characterCenter = GetPosition() + GetSize() * 0.5f;
		int id = this->health->GetTotalHealthBar().GetID();
		assert(relativeCenterRatios.count(id) > 0 && "The relative center ratio for the total health bar is not set");
		assert(relativeCenterRatios[id].count(GameCharacterState::FIGHTING) > 0 && "The relative center ratio for the total health bar in the fighting state is not set");
		glm::vec2 relativeCenterRatio = relativeCenterRatios[id][GameCharacterState::FIGHTING];
		glm::vec2 totalHealthBarCenter = characterCenter + relativeCenterRatio * GetSize();
		this->health->GetTotalHealthBar().SetCenter(totalHealthBarCenter);
		id = this->health->GetCurrentHealthBar().GetID();
		assert(relativeCenterRatios.count(id) > 0 && "The relative center ratio for the current health bar is not set");
		assert(relativeCenterRatios[id].count(GameCharacterState::FIGHTING) > 0 && "The relative center ratio for the current health bar in the fighting state is not set");
		relativeCenterRatio = relativeCenterRatios[id][GameCharacterState::FIGHTING];
		glm::vec2 currentHealthBarCenter = characterCenter + relativeCenterRatio * GetSize();
		this->health->GetCurrentHealthBar().SetCenter(currentHealthBarCenter);
	}
}

void GameCharacter::ClearCarriedObjects() {
	for(auto& object : carriedObjects) {
		int id = object.first;
		relativeCenterRatios.erase(id);
	}
	carriedObjects.clear();
}


