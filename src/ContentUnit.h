#pragma once
#include <string>
#include "Text.h"
#include "Button.h"

// Enum for the type of content unit
enum class ContentType {
	kText,
	kButton,
	kImage,
	kOption
};

// Enum for the state of the option unit
enum class OptionState {
	kNormal,
	kHovered,
	kClicked,
	kUnclickable
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
	TextUnit(const std::string& name, std::shared_ptr<Text> text, std::shared_ptr<TextRenderer> textRenderer, bool isCentered = false);
	std::shared_ptr<Text> GetText();
	void SetText(std::shared_ptr<Text> text);
	std::shared_ptr<TextRenderer> GetTextRenderer();
	void SetTextRenderer(std::shared_ptr<TextRenderer> textRenderer);
	void UpdateHeight() override;
	void SetPosition(glm::vec2 pos) override;
	glm::vec2 GetPosition() const override;
	void SetCentered(bool isCentered);
	bool IsCentered() const;
	void Draw() override;
private:
	std::shared_ptr<Text> text_;
	std::shared_ptr<TextRenderer> text_renderer_;
	bool is_centered_;
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
	std::shared_ptr<TextRenderer> text_renderer_;
	std::shared_ptr<ColorRenderer> color_renderer_; 
};

class ImageUnit : public ContentUnit {
public:
	ImageUnit(const std::string& name, glm::vec2 position, glm::vec2 size, const Texture2D& sprite, std::shared_ptr<SpriteRenderer> spriteRenderer, 
		float rotation = 0.f, glm::vec2 rotationPivot = glm::vec2(0.5f, 0.5f), glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), TextureRenderingMode mode = TextureRenderingMode::kNormal);
	void UpdateHeight() override;
	void SetPosition(glm::vec2 pos) override;
	glm::vec2 GetPosition() const override;
	void SetSize(glm::vec2 size);
	glm::vec2 GetSize() const;
	void SetColor(glm::vec4 color);
	glm::vec4 GetColor() const;
	void SetTextureRenderingMode(TextureRenderingMode mode);
	TextureRenderingMode GetTextureRenderingMode() const;
	void Draw() override;
private:
	Texture2D  sprite_;
	glm::vec2  position_;
	glm::vec2  size_;
	float      rotation_;
	glm::vec2  rotation_pivot_;
	glm::vec4  color_;
	TextureRenderingMode mode_;
	std::shared_ptr<SpriteRenderer> sprite_renderer_;
};

// OptionUnit is a ContentUnit that used to choose an option, such as windowed mode or full screen mode. Each option has one icon image and one line of text.
class OptionUnit : public ContentUnit {
public:
	OptionUnit(const std::string& name, std::shared_ptr<ImageUnit> icon, std::shared_ptr<TextUnit> text, OptionState state = OptionState::kUnclickable, bool imageOnLeft = true, bool textOnCenter = false);
	std::shared_ptr<ImageUnit> GetIcon();
	void SetIcon(std::shared_ptr<ImageUnit> icon);
	std::shared_ptr<TextUnit> GetText();
	void SetText(std::shared_ptr<TextUnit> text);
	void UpdateHeight() override;
	void SetPosition(glm::vec2 pos) override;
	glm::vec2 GetPosition() const override;
	void SetHorizontalSpacing(float spacing);
	float GetHorizontalSpacing() const;
	void SetState(OptionState state);
	OptionState GetState() const;
	void SetImageOnLeft(bool imageOnLeft);
	bool IsImageOnLeft() const;
	void SetTextOnCenter(bool textOnCenter);
	bool IsTextOnCenter() const;
	bool IsPositionInsideIcon(glm::vec2 position) const;
	void Draw() override;
private:
	std::shared_ptr<ImageUnit> icon_;
	std::shared_ptr<TextUnit> text_;
	float horizontal_spacing_;
	OptionState state_;
	bool image_on_left_;
	bool text_on_center_;
};
