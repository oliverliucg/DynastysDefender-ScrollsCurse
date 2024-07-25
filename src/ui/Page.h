#pragma once
#include "Capsule.h"
#include "PageSection.h"
#include "Text.h"

class Page {
 public:
  Page(const std::string& name);
  Page() = delete;
  std::string GetName() const;
  void UpdateComponentsHeight();
  void SetCompenentsTextRenderer(std::shared_ptr<TextRenderer> textRenderer);
  float GetHeight() const;
  std::shared_ptr<PageSection> GetSection(const std::string& name);
  void AddSection(std::shared_ptr<PageSection> section);
  void RemoveSection(const std::string& name);
  std::vector<std::string> GetOrder() const;
  void SetOrder(const std::vector<std::string>& order);
  void SetInterSectionSpacing(const std::string& section1,
                              const std::string& section2, float spacing);
  float GetInterSectionSpacing(const std::string& section1,
                               const std::string& section2) const;
  void SetTopSpacing(float spacing);
  float GetTopSpacing() const;
  void SetBottomSpacing(float spacing);
  float GetBottomSpacing() const;
  void SetLeftSpacing(float spacing);
  float GetLeftSpacing() const;
  void SetMaxHeight(float height);
  float GetMaxHeight() const;
  void SetPosition(glm::vec2 pos);
  void UpdatePosition();
  glm::vec2 GetPosition() const;
  void Draw();

 private:
  std::string name_{""};
  glm::vec2 position_{0.f, 0.f};
  std::unordered_map<std::string, std::shared_ptr<PageSection>> sections_;
  std::vector<std::string> order_;
  std::unordered_map<std::string, std::unordered_map<std::string, float>>
      inter_section_spacing_;
  float top_spacing_{0.f};
  float bottom_spacing_{0.f};
  float left_spacing_{0.f};
  float max_height_{std::numeric_limits<float>::max()};
  float GetLimitedSectionHeight(const std::string& section_name) const;
};