#include "Page.h"

Page::Page(const std::string& name)
    : name_(name),
      position_(glm::vec2(0.f)),
      top_spacing_(0.f),
      bottom_spacing_(0.f),
      left_spacing_(0.f),
      max_height_(std::numeric_limits<float>::max()) {}

std::string Page::GetName() const { return name_; }

float Page::GetLimitedSectionHeight(const std::string& section_name) const {
  assert(sections_.find(section_name) != sections_.end() &&
         "Section not found");
  return std::min(sections_.at(section_name)->GetHeight(),
                  sections_.at(section_name)->GetMaxHeight());
}

void Page::UpdateComponentsHeight() {
  for (size_t i = 0; i < order_.size(); ++i) {
    assert(sections_.find(order_[i]) != sections_.end() && "Section not found");
    sections_[order_[i]]->UpdateComponentsHeight();
  }
}

void Page::SetCompenentsTextRenderer(
    std::shared_ptr<TextRenderer> textRenderer) {
  for (size_t i = 0; i < order_.size(); ++i) {
    assert(sections_.find(order_[i]) != sections_.end() && "Section not found");
    sections_[order_[i]]->SetCompenentsTextRenderer(textRenderer);
  }
}

float Page::GetHeight() const {
  float height = top_spacing_ + bottom_spacing_;
  for (size_t i = 0; i < order_.size(); ++i) {
    assert(sections_.find(order_[i]) != sections_.end() && "Section not found");
    height += GetLimitedSectionHeight(order_[i]);
    if (i + 1 < order_.size()) {
      if (inter_section_spacing_.find(order_[i]) ==
              inter_section_spacing_.end() ||
          inter_section_spacing_.at(order_[i]).find(order_[i + 1]) ==
              inter_section_spacing_.at(order_[i]).end()) {
        continue;
      }
      height += inter_section_spacing_.at(order_[i]).at(order_[i + 1]);
    }
  }
  return height;
}

std::shared_ptr<PageSection> Page::GetSection(const std::string& name) {
  assert(sections_.find(name) != sections_.end() && "Section not found");
  return sections_[name];
}

void Page::AddSection(std::shared_ptr<PageSection> section) {
  sections_[section->GetName()] = section;
  order_.push_back(section->GetName());
}

void Page::RemoveSection(const std::string& name) {
  assert(sections_.find(name) != sections_.end() && "Section not found");
  sections_.erase(name);
  order_.erase(std::remove(order_.begin(), order_.end(), name), order_.end());
  // remove inter_section_spacing
  inter_section_spacing_.erase(name);
  for (auto& spacing : inter_section_spacing_) {
    spacing.second.erase(name);
  }
}

std::vector<std::string> Page::GetOrder() const { return order_; }

void Page::SetOrder(const std::vector<std::string>& order) {
  this->order_ = order;
}

void Page::SetInterSectionSpacing(const std::string& section1,
                                  const std::string& section2, float spacing) {
  inter_section_spacing_[section1][section2] = spacing;
  inter_section_spacing_[section2][section1] = spacing;
}

float Page::GetInterSectionSpacing(const std::string& section1,
                                   const std::string& section2) const {
  return inter_section_spacing_.at(section1).at(section2);
}

void Page::SetTopSpacing(float spacing) { top_spacing_ = spacing; }

float Page::GetTopSpacing() const { return top_spacing_; }

void Page::SetBottomSpacing(float spacing) { bottom_spacing_ = spacing; }

float Page::GetBottomSpacing() const { return bottom_spacing_; }

void Page::SetLeftSpacing(float spacing) { left_spacing_ = spacing; }

float Page::GetLeftSpacing() const { return left_spacing_; }

void Page::SetMaxHeight(float height) { max_height_ = height; }

float Page::GetMaxHeight() const { return max_height_; }

void Page::SetPosition(glm::vec2 pos) {
  float x = pos.x + left_spacing_;
  float y = pos.y + top_spacing_;
  for (size_t i = 0; i < order_.size(); ++i) {
    assert(sections_.find(order_[i]) != sections_.end() && "Section not found");
    sections_[order_[i]]->SetPosition(glm::vec2(x, y));
    y += GetLimitedSectionHeight(order_[i]);
    if (i + 1 < order_.size()) {
      y += inter_section_spacing_[order_[i]][order_[i + 1]];
    }
  }
  position_ = pos;
}

void Page::UpdatePosition() { SetPosition(GetPosition()); }

glm::vec2 Page::GetPosition() const { return position_; }

void Page::Draw() {
  for (size_t i = 0; i < order_.size(); ++i) {
    assert(sections_.find(order_[i]) != sections_.end() && "Section not found");
    sections_[order_[i]]->Draw();
  }
}
