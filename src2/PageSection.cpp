#include "PageSection.h"

const float PageSection::kScrollIconWidth = 0.5f * kBubbleRadius;

PageSection::PageSection(const std::string& name) : name_(name), position_(glm::vec2(0.f)), top_spacing_(0.0f), bottom_spacing_(0.0f), left_spacing_(0.0f), max_height_(kWindowSize.y), max_width_(kWindowSize.x), offset_(0.f), scroll_relation_(glm::vec3(0.f)), scroll_icon_(nullptr) {}

std::string PageSection::GetName() const {
	return name_;
}

float PageSection::GetHeight() const {
	float height = top_spacing_ + bottom_spacing_;
	for (size_t i = 0; i < order_.size(); ++i) {
		assert(content_.find(order_[i]) != content_.end() && "Content not found in PageSection!");
		height += content_.at(order_[i])->GetHeight();
		if (i + 1 < order_.size()) {
			if (inter_unit_spacing_.find(order_[i]) == inter_unit_spacing_.end() || 
				inter_unit_spacing_.at(order_[i]).find(order_[i + 1]) == inter_unit_spacing_.at(order_[i]).end()) {
				continue;
			}
			height += inter_unit_spacing_.at(order_[i]).at(order_[i+1]);
		}
	}
	return height;
}

void PageSection::SetMaxHeight(float height) {
	max_height_ = height;
}

float PageSection::GetMaxHeight() const {
	return max_height_;
}

void PageSection::SetMaxWidth(float width) {
	max_width_ = width;
	for (const auto& unit : order_) {
		if (content_[unit]->GetType() == ContentType::kText) {
			std::dynamic_pointer_cast<TextUnit>(content_[unit])->GetText()->SetLineWidth(max_width_ - left_spacing_);
		}
	}
}

float PageSection::GetMaxWidth() const {
	return max_width_;
}

void PageSection::SetOffset(float offset) {
	offset_ = offset;
}

float PageSection::GetOffset() const {
	return offset_;
}

glm::vec4 PageSection::GetBoundingBox() const {
	float topLeftX = position_.x;
	float topLeftY = position_.y;
	float bottomRightX = position_.x + max_width_;
	float bottomRightY = position_.y + std::min(GetHeight(), max_height_);
	if (scroll_icon_ != nullptr) {
		bottomRightX = std::max(bottomRightX, scroll_icon_->GetPosition().x + scroll_icon_->GetSize().x);
		bottomRightY = std::max(bottomRightY, scroll_icon_->GetPosition().y + scroll_icon_->GetSize().y);
	}
	return glm::vec4(topLeftX, topLeftY, bottomRightX, bottomRightY);
}

std::shared_ptr<ContentUnit> PageSection::GetContent(const std::string& name) {
	assert(content_.find(name) != content_.end() && "Content not found in PageSection!");
	return content_[name];
}


void PageSection::AddContent(std::shared_ptr<ContentUnit> unit) {
	content_[unit->GetName()] = unit;
	order_.push_back(unit->GetName());
}

void PageSection::RemoveContent(const std::string& name) {
	assert(content_.find(name) != content_.end() && "Content not found in PageSection!");
	content_.erase(name);
	order_.erase(std::remove(order_.begin(), order_.end(), name), order_.end());
	// remove inter_unit_spacing
	inter_unit_spacing_.erase(name);
	for (auto& spacing : inter_unit_spacing_) {
		spacing.second.erase(name);
	}
}

std::vector<std::string> PageSection::GetOrder() const {
	return order_;
}

void PageSection::SetOrder(const std::vector<std::string>& order) {
	this->order_ = order;
}

void PageSection::SetInterUnitSpacing(const std::string& unit1, const std::string& unit2, float spacing) {
	inter_unit_spacing_[unit1][unit2] = spacing;
	inter_unit_spacing_[unit2][unit1] = spacing;
}

void PageSection::SetTopSpacing(float spacing) {
	top_spacing_ = spacing;
}

float PageSection::GetTopSpacing() const {
	return top_spacing_;
}

void PageSection::SetBottomSpacing(float spacing) {
	bottom_spacing_ = spacing;
}

float PageSection::GetBottomSpacing() const {
	return bottom_spacing_;
}

void PageSection::SetLeftSpacing(float spacing) {
	left_spacing_ = spacing;
}

float PageSection::GetLeftSpacing() const {
	return left_spacing_;
}

void PageSection::SetPosition(glm::vec2 pos) {
	float x = pos.x + left_spacing_;
	float y = pos.y + top_spacing_;
	for (size_t i = 0; i < order_.size(); ++i) {
		assert(content_.find(order_[i]) != content_.end() && "Content not found in PageSection!");
		content_[order_[i]]->SetPosition(glm::vec2(x, y));
		y += content_[order_[i]]->GetHeight();
		if (i + 1 < order_.size()) {
			y += inter_unit_spacing_[order_[i]][order_[i+1]];
		}
	}
	position_ = pos;
}

void PageSection::UpdatePosition() {
	SetPosition(GetPosition());
}

glm::vec2 PageSection::GetPosition() const {
	return position_;
}

void PageSection::InitScrollIcon(std::shared_ptr<ColorRenderer> colorRenderer, std::shared_ptr<CircleRenderer> circleRenderer, std::shared_ptr<LineRenderer> lineRenderer, float scrollIconCenterX) {
	// Get the height of the section.
	float height = GetHeight();
	// If height is less than max_height_, set the scroll icon to nullptr, otherwise update the scroll icon.
	if (height <= max_height_) {
		scroll_icon_ = nullptr;
		return;
	}
	scroll_icon_ = std::make_unique<Capsule>();
	// Set the size of the scroll icon. The height of the scroll icon is based on the difference between height and max_height_. The minimum height of the scroll icon is 0.8 * kBubbleRadius.
	float scroll_height = std::max(0.8f * kBubbleRadius, (max_height_ / height) * max_height_);
	scroll_icon_->SetSize(glm::vec2(PageSection::kScrollIconWidth, scroll_height));
	// Set the center of the icon to be at the right bottom of the text box.
	//float scrollCenterX = this->GetPosition().x + this->GetMaxWidth();
	scroll_icon_->SetCenter(glm::vec2(scrollIconCenterX, this->GetPosition().y+scroll_icon_->GetSize().y*0.5f));
	// Set the color of the scroll icon.
	scroll_icon_->SetColor(glm::vec4(0.75294f, 0.43922f, 0.03922f, 1.0f));
	// Set all parts of icon to be visible.
	scroll_icon_->SetRectangleVisible(true);
	scroll_icon_->SetTopSemiCircleVisible(true);
	scroll_icon_->SetBottomSemiCircleVisible(true);
	// Add top and bottom boundart lines for the scroll icon.
	// Top line
	lines_.clear();
	glm::vec2 point1(scroll_icon_->GetPosition().x, this->GetPosition().y);
	glm::vec2 point2(scroll_icon_->GetPosition().x + scroll_icon_->GetSize().x, this->GetPosition().y);
	lines_.emplace_back(point1);
	lines_.emplace_back(point2);
	// Bottom line
	point1 = glm::vec2(scroll_icon_->GetPosition().x, this->GetPosition().y + this->GetMaxHeight());
	point2 = glm::vec2(scroll_icon_->GetPosition().x + scroll_icon_->GetSize().x, this->GetPosition().y + this->GetMaxHeight());
	lines_.emplace_back(point1);
	lines_.emplace_back(point2);
	// Get the relationship between the offset of scroll icon and the offset of the content in the section.
	glm::vec2 relationshipPoint1 = glm::vec2(0.f, 0.f);
	glm::vec2 relationshipPoint2 = glm::vec2(lines_[2].y - lines_[0].y - scroll_icon_->GetSize().y, 0.8f*max_height_-this->GetHeight());
	scroll_relation_ = solveLine(relationshipPoint1, relationshipPoint2);
	this->SetScrollRelationShip(scroll_relation_);

	// Set renderers
	color_renderer_ = colorRenderer;
	circle_renderer_ = circleRenderer;
	line_renderer_ = lineRenderer;
}

void PageSection::SetScrollRelationShip(glm::vec3 relation) {
	scroll_relation_ = relation;
}

glm::vec3 PageSection::GetScrollRelationShip() const{
	return scroll_relation_;
}

Capsule& PageSection::GetScrollIcon() {
	if (scroll_icon_ == nullptr) {
		scroll_icon_ = std::make_unique<Capsule>();
	}
	return *scroll_icon_;
}

void PageSection::MoveScrollIcon(float scroll_y_offset) {
	assert(scroll_icon_ != nullptr && "Scroll icon is not initialized!");
	// Get the bounding box of the section.
	scroll_icon_->Move(glm::vec2(0.f, scroll_y_offset), GetBoundingBox());
	// Update the offset of the content in the section based on the relationship between the offset of scroll icon and the offset of the content in the section
	glm::vec2 position = scroll_icon_->GetPosition();
	glm::vec2 size = scroll_icon_->GetSize();
	glm::vec3 relation = GetScrollRelationShip();
	float oldOffset = GetOffset();
	float newOffset = getYOfLine(position.y - lines_[0].y, relation);
	SetOffset(newOffset);
}

bool PageSection::NeedScrollIcon() {
	float height = GetHeight();
	return height > max_height_;
}

bool PageSection::IsScrollIconInitialized() {
	return scroll_icon_ != nullptr;
}

bool PageSection::IsMouseInBox(glm::vec2 mouse_position) {
	glm::vec4 boundingBox = GetBoundingBox();
	return mouse_position.x > boundingBox.x && mouse_position.x < boundingBox.z && mouse_position.y > boundingBox.y && mouse_position.y < boundingBox.w;
}

void PageSection::Draw() {
	ScissorBoxHandler& handler = ScissorBoxHandler::GetInstance();
	bool scissorTestEnabled = handler.IsScissorTestEnabled();
	handler.EnableScissorTest();
	glm::vec4 boundingBox = GetBoundingBox();
	GLsizei scissorBoxWidth = std::ceil(boundingBox.z - boundingBox.x);
	GLsizei scissorBoxHeight = std::ceil(boundingBox.w - boundingBox.y);
	ScissorBoxHandler::ScissorBox sectionScissorBox(boundingBox.x, kWindowSize.y-boundingBox.w, scissorBoxWidth, scissorBoxHeight);
	/*std::cout << "page section name: " << name_ << std::endl;*/
	handler.SetIntersectedScissorBox(sectionScissorBox);
	for (size_t i = 0; i < order_.size(); ++i) {
		assert(content_.find(order_[i]) != content_.end() && "Content not found in PageSection!");
		content_[order_[i]]->SetPosition(glm::vec2(content_[order_[i]]->GetPosition().x, content_[order_[i]]->GetPosition().y + offset_));
		content_[order_[i]]->Draw();
		content_[order_[i]]->SetPosition(glm::vec2(content_[order_[i]]->GetPosition().x, content_[order_[i]]->GetPosition().y - offset_));
	}
	// Restore the scissor box
	handler.RestoreScissorBox();
	if (!scissorTestEnabled) {
		handler.DisableScissorTest();
	}

	// Draw the scroll icon if it is initialized.
	if (IsScrollIconInitialized()) {
		scroll_icon_->Draw(color_renderer_, circle_renderer_);
		line_renderer_->DrawLines(lines_, glm::vec4(0.8f, 0.62353f, 0.54902f, 1.0f));
	}
}