#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cassert>
#include "ContentUnit.h"
#include "Capsule.h"
#include "ScissorBoxHandler.h"

class PageSection {
public:
	PageSection(const std::string& name);
	PageSection() = delete;
	std::string GetName() const;
	float GetHeight() const;
	std::shared_ptr<ContentUnit> GetContent(const std::string& name);
	void AddContent(std::shared_ptr<ContentUnit> unit);
	void RemoveContent(const std::string& name);
	std::vector<std::string> GetOrder() const;
	void SetOrder(const std::vector<std::string>& order);
	void SetInterUnitSpacing(const std::string& unit1, const std::string& unit2, float spacing);
	void SetTopSpacing(float spacing);
	float GetTopSpacing() const;
	void SetBottomSpacing(float spacing);
	float GetBottomSpacing() const;
	void SetLeftSpacing(float spacing);
	float GetLeftSpacing() const;
	void SetPosition(glm::vec2 pos);
	void UpdatePosition();
	glm::vec2 GetPosition() const;
	void SetMaxHeight(float height);
	float GetMaxHeight() const;
	void SetMaxWidth(float width);
	float GetMaxWidth() const;
	void SetOffset(float offset);
	float GetOffset() const;
	void SetScrollIconAllowed(bool is_allowed);
	bool IsScrollIconAllowed() const;
	glm::vec4 GetBoundingBox() const;
	void InitScrollIcon(std::shared_ptr<ColorRenderer> colorRenderer, std::shared_ptr<CircleRenderer> circleRenderer, std::shared_ptr<LineRenderer> lineRenderer, float scrollIconCenterX);
	void ResetSrcollIconPosition();
	void SetScrollRelationShip(glm::vec3 relation);
	glm::vec3 GetScrollRelationShip() const;
	Capsule& GetScrollIcon();
	void MoveScrollIcon(float scroll_y_offset);
	bool NeedScrollIcon();
	bool IsScrollIconInitialized();
	bool IsMouseInBox(glm::vec2 mouse_position);
	void Draw();

	static const float kScrollIconWidth;

private:
	std::string name_;
	glm::vec2 position_;
	std::unordered_map<std::string, std::shared_ptr<ContentUnit>> content_;
	std::vector<std::string> order_;
	// space between content units
	std::unordered_map<std::string, std::unordered_map<std::string, float>> inter_unit_spacing_;
	// space between the top of the section and the first content unit
	float top_spacing_;
	// space between the bottom of the section and the last content unit
	float bottom_spacing_;
	// space between the left of the section and the content units
	float left_spacing_;
	// The maximum height of the section.
	float max_height_;
	// The maximum width of the section.
	float max_width_;
	// offset while rendering the components.
	float offset_;
	// scroll icon is allowed or not.
	bool is_scroll_icon_allowed_;
	// The linear relationship between the center of the scroll icon and the position of content in the section.
	glm::vec3 scroll_relation_;
	// Scroll icon. a capsule with two semicircles and a rectangle.
	std::unique_ptr<Capsule> scroll_icon_;
	// Lines to be drawn withing the section.
	std::vector<glm::vec2> lines_;
	// Renderers
	std::shared_ptr<ColorRenderer> color_renderer_;
	std::shared_ptr<CircleRenderer> circle_renderer_;
	std::shared_ptr<LineRenderer> line_renderer_;
};