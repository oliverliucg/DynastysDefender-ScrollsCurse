#include "ContentUnit.h"

ContentUnit::ContentUnit(ContentType type, float height, const std::string& name) : type_(type), height_(height), name_(name) {}

ContentType ContentUnit::GetType() const {
	return type_;
}

float ContentUnit::GetHeight() const {
	return height_;
}

std::string ContentUnit::GetName() const {
	return name_;
}

void ContentUnit::SetName(const std::string& name) {
	name_ = name;
}

TextUnit::TextUnit(const std::string& name, std::shared_ptr<Text> text, std::shared_ptr<TextRenderer> textRenderer) : ContentUnit(ContentType::kText, text->GetTextSize(textRenderer).y, name), text_(text), textRenderer_(textRenderer) {}

std::shared_ptr<Text> TextUnit::GetText() {
	return text_;
}

void TextUnit::SetText(std::shared_ptr<Text> text) {
	text_ = text;
	// Update height
	height_ = text_->GetTextSize(textRenderer_).y;
}

std::shared_ptr<TextRenderer> TextUnit::GetTextRenderer() {
	return textRenderer_;
}

void TextUnit::SetTextRenderer(std::shared_ptr<TextRenderer> textRenderer) {
	textRenderer_ = textRenderer;
	// Update height
	height_ = text_->GetTextSize(textRenderer_).y;
}

void TextUnit::UpdateHeight() {
	std::cout << "old height: " << height_ << std::endl;
	height_ = text_->GetTextSize(textRenderer_).y;
	std::cout << "new height: " << height_ << std::endl;
}

void TextUnit::SetPosition(glm::vec2 pos) {
	text_->SetPosition(pos);
}

glm::vec2 TextUnit::GetPosition() const {
	return text_->GetPosition();
}

void TextUnit::Draw() {
	text_->Draw(textRenderer_);
}

ButtonUnit::ButtonUnit(const std::string& name, std::shared_ptr<Button> button, std::shared_ptr<TextRenderer> textRenderer, std::shared_ptr<ColorRenderer> colorRenderer):
	ContentUnit(ContentType::kButton, button->GetSize().y, name), button_(button), 
	textRenderer_(textRenderer), colorRenderer_(colorRenderer){}

std::shared_ptr<Button> ButtonUnit::GetButton() {
	return button_;
}

void ButtonUnit::SetButton(std::shared_ptr<Button> button) {
	button_ = button;
	// Update height
	height_ = button_->GetSize().y;
}

void ButtonUnit::SetRenderer(std::shared_ptr<TextRenderer> textRenderer, std::shared_ptr<ColorRenderer> colorRenderer) {
	textRenderer_ = textRenderer;
	colorRenderer_ = colorRenderer;
	// Update height
	height_ = button_->GetSize().y;
}

void ButtonUnit::UpdateHeight() {
	height_ = button_->GetSize().y;
}

void ButtonUnit::SetPosition(glm::vec2 pos) {
	button_->SetPosition(pos);
}

glm::vec2 ButtonUnit::GetPosition() const {
	return button_->GetPosition();
}

void ButtonUnit::SetSize(glm::vec2 size) {
	button_->SetSize(size);
	// Update height
	height_ = button_->GetSize().y;
}

glm::vec2 ButtonUnit::GetSize() const {
	return button_->GetSize();
}

void ButtonUnit::Draw() {
	button_->Draw(textRenderer_, colorRenderer_);
}