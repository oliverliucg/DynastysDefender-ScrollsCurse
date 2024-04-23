#pragma once
#include <string>
#include "Text.h"
#include "Button.h"

// Enum for the type of content unit
enum class ContentType {
	kText,
	kButton
};

class ContentUnit {
public:
	ContentType GetType() const;
	float GetHeight() const;
	std::string GetName() const;
	void SetName(const std::string& name);
	virtual void UpdateHeight() = 0;
	virtual void SetPosition(glm::vec2 pos) = 0;
	virtual glm::vec2 GetPosition() const = 0;
	virtual void Draw() = 0;

protected:
	ContentUnit(ContentType type, float height, const std::string& name);
	ContentType type_;
	float height_;
	std::string name_;
};


class TextUnit : public ContentUnit {
public:
	TextUnit(const std::string& name, std::shared_ptr<Text> text, std::shared_ptr<TextRenderer> textRenderer);
	std::shared_ptr<Text> GetText();
	void SetText(std::shared_ptr<Text> text);
	void SetTextRenderer(std::shared_ptr<TextRenderer> textRenderer);
	void UpdateHeight() override;
	void SetPosition(glm::vec2 pos) override;
	glm::vec2 GetPosition() const override;
	void Draw() override;
private:
	std::shared_ptr<Text> text_;
	std::shared_ptr<TextRenderer> textRenderer_;
};

class ButtonUnit : public ContentUnit {
public:
	ButtonUnit(const std::string& name, std::shared_ptr<Button> button, std::shared_ptr<TextRenderer> textRenderer, std::shared_ptr<ColorRenderer> colorRenderer);
	std::shared_ptr<Button> GetButton();
	void SetButton(std::shared_ptr<Button> button);
	void SetRenderer(std::shared_ptr<TextRenderer> textRenderer, std::shared_ptr<ColorRenderer> colorRenderer);
	void UpdateHeight() override;
	void SetPosition(glm::vec2 pos) override;
	glm::vec2 GetPosition() const override;
	void SetSize(glm::vec2 size);
	glm::vec2 GetSize() const;
	void Draw() override;
private:
	std::shared_ptr<Button> button_;
	std::shared_ptr<TextRenderer> textRenderer_;
	std::shared_ptr<ColorRenderer> colorRenderer_; 
};
