#include "ContentUnit.h"

ContentUnit::ContentUnit(ContentType type, float height,
                         const std::string& name)
    : type_(type), height_(height), name_(name) {}

ContentType ContentUnit::GetType() const { return type_; }

float ContentUnit::GetHeight() const { return height_; }

std::string ContentUnit::GetName() const { return name_; }

void ContentUnit::SetName(const std::string& name) { name_ = name; }

TextUnit::TextUnit(const std::string& name, std::shared_ptr<Text> text,
                   std::shared_ptr<TextRenderer> textRenderer, bool isCentered)
    : ContentUnit(ContentType::kText, text->GetTextSize(textRenderer).y, name),
      text_(text),
      text_renderer_(textRenderer),
      is_centered_(isCentered) {}

std::shared_ptr<Text> TextUnit::GetText() { return text_; }

void TextUnit::SetText(std::shared_ptr<Text> text) {
  text_ = text;
  // Update height
  height_ = text_->GetTextSize(text_renderer_).y;
}

std::shared_ptr<TextRenderer> TextUnit::GetTextRenderer() {
  return text_renderer_;
}

void TextUnit::SetTextRenderer(std::shared_ptr<TextRenderer> textRenderer) {
  text_renderer_ = textRenderer;
  // Update height
  height_ = text_->GetTextSize(text_renderer_).y;
}

void TextUnit::UpdateHeight() {
  height_ = text_->GetTextSize(text_renderer_).y;
}

void TextUnit::SetPosition(glm::vec2 pos) { text_->SetPosition(pos); }

glm::vec2 TextUnit::GetPosition() const { return text_->GetPosition(); }

void TextUnit::SetCentered(bool isCentered) { is_centered_ = isCentered; }

bool TextUnit::IsCentered() const { return is_centered_; }

void TextUnit::Draw() { text_->Draw(text_renderer_, is_centered_); }

ButtonUnit::ButtonUnit(const std::string& name, std::shared_ptr<Button> button,
                       std::shared_ptr<TextRenderer> textRenderer,
                       std::shared_ptr<ColorRenderer> colorRenderer)
    : ContentUnit(ContentType::kButton, button->GetSize().y, name),
      button_(button),
      text_renderer_(textRenderer),
      color_renderer_(colorRenderer) {}

std::shared_ptr<Button> ButtonUnit::GetButton() { return button_; }

void ButtonUnit::SetButton(std::shared_ptr<Button> button) {
  button_ = button;
  // Update height
  height_ = button_->GetSize().y;
}

void ButtonUnit::SetRenderer(std::shared_ptr<TextRenderer> textRenderer,
                             std::shared_ptr<ColorRenderer> colorRenderer) {
  text_renderer_ = textRenderer;
  color_renderer_ = colorRenderer;
  // Update height
  height_ = button_->GetSize().y;
}

void ButtonUnit::UpdateHeight() { height_ = button_->GetSize().y; }

void ButtonUnit::SetPosition(glm::vec2 pos) { button_->SetPosition(pos); }

glm::vec2 ButtonUnit::GetPosition() const { return button_->GetPosition(); }

void ButtonUnit::SetSize(glm::vec2 size) {
  button_->SetSize(size);
  // Update height
  height_ = button_->GetSize().y;
}

glm::vec2 ButtonUnit::GetSize() const { return button_->GetSize(); }

void ButtonUnit::Draw() { button_->Draw(text_renderer_, color_renderer_); }

ImageUnit::ImageUnit(const std::string& name, glm::vec2 position,
                     glm::vec2 size, const Texture2D& sprite,
                     std::shared_ptr<SpriteRenderer> spriteRenderer,
                     float rotation, glm::vec2 rotationPivot, glm::vec4 color,
                     TextureRenderingMode mode)
    : ContentUnit(ContentType::kImage, size.y, name),
      sprite_(sprite),
      position_(position),
      size_(size),
      rotation_(rotation),
      rotation_pivot_(rotationPivot),
      color_(color),
      mode_(mode),
      sprite_renderer_(spriteRenderer) {}

void ImageUnit::UpdateHeight() { height_ = size_.y; }

void ImageUnit::SetPosition(glm::vec2 pos) { position_ = pos; }

glm::vec2 ImageUnit::GetPosition() const { return position_; }

void ImageUnit::SetSize(glm::vec2 size) {
  size_ = size;
  // Update height
  height_ = size_.y;
}

glm::vec2 ImageUnit::GetSize() const { return size_; }

void ImageUnit::SetColor(glm::vec4 color) { color_ = color; }

glm::vec4 ImageUnit::GetColor() const { return color_; }

void ImageUnit::SetTextureRenderingMode(TextureRenderingMode mode) {
  mode_ = mode;
}

TextureRenderingMode ImageUnit::GetTextureRenderingMode() const {
  return mode_;
}

void ImageUnit::Draw() {
  sprite_renderer_->DrawSprite(sprite_, position_, size_, rotation_,
                               rotation_pivot_, color_, mode_);
}

OptionUnit::OptionUnit(const std::string& name, std::shared_ptr<ImageUnit> icon,
                       std::shared_ptr<TextUnit> text, OptionState state,
                       bool imageOnLeft, bool textOnCenter)
    : ContentUnit(ContentType::kOption, text->GetHeight(), name),
      icon_(icon),
      text_(text),
      horizontal_spacing_(text->GetTextRenderer()
                              ->characterMap.at(CharStyle::Regular)
                              .at('N')
                              .Size.x *
                          text->GetText()->GetScale()),
      state_(state),
      image_on_left_(imageOnLeft),
      text_on_center_(textOnCenter) {
  // horizontal_spacing_ -=
  // text->GetTextRenderer()->characterMap.at(CharStyle::Regular).at('N').Bearing.x
  // * text->GetText()->GetScale(); float AdvanceN =
  // text->GetTextRenderer()->characterMap.at(CharStyle::Regular).at('N').Advance
  // >> 6; float BearingN =
  // text->GetTextRenderer()->characterMap.at(CharStyle::Regular).at('N').Bearing.x;
  // float charSizeN =
  // text->GetTextRenderer()->characterMap.at(CharStyle::Regular).at('N').Size.x;
  // horizontal_spacing_ -= (AdvanceN - BearingN - charSizeN) *
  // text->GetText()->GetScale(); std::cout << "horizontal spacing: " <<
  // horizontal_spacing_ << std::endl; float horizontal_spacing2 = charSizeN *
  // text->GetText()->GetScale(); std::cout << "horizontal spacing2: " <<
  // horizontal_spacing2 << std::endl;
  /*this->SetState(this->state_);*/
  char firstChar = text->GetText()->GetParagraph(0)[0];
  horizontal_spacing_ -= text->GetTextRenderer()
                             ->characterMap.at(CharStyle::Regular)
                             .at(firstChar)
                             .Bearing.x *
                         text->GetText()->GetScale();
  this->SetPosition(this->GetPosition());
}

std::shared_ptr<ImageUnit> OptionUnit::GetIcon() { return icon_; }

void OptionUnit::SetIcon(std::shared_ptr<ImageUnit> icon) {
  icon_ = icon;
  // Update height
  UpdateHeight();
}

std::shared_ptr<TextUnit> OptionUnit::GetText() { return text_; }

void OptionUnit::SetText(std::shared_ptr<TextUnit> text) {
  text_ = text;
  // Update height
  UpdateHeight();
}

void OptionUnit::UpdateHeight() {
  icon_->UpdateHeight();
  text_->UpdateHeight();
  height_ = std::max(icon_->GetHeight(), text_->GetHeight());
}

void OptionUnit::SetPosition(glm::vec2 pos) {
  if (image_on_left_) {
    icon_->SetPosition(pos);
    text_->SetPosition(pos +
                       glm::vec2(icon_->GetSize().x + horizontal_spacing_, 0));
  } else {
    text_->SetPosition(pos);
    icon_->SetPosition(
        pos +
        glm::vec2(text_->GetText()->GetTextSize(text_->GetTextRenderer()).x +
                      horizontal_spacing_,
                  0));
  }
  if (text_on_center_) {
    glm::vec2 textSize =
        text_->GetText()->GetTextSize(text_->GetTextRenderer());
    glm::vec2 imageSize = icon_->GetSize();
    float offsetY = (imageSize.y - textSize.y) / 2.0f;
    float textY = pos.y + offsetY;
    text_->SetPosition(glm::vec2(text_->GetPosition().x, textY));
    // glm::vec2 imageCenter = icon_->GetPosition() + icon_->GetSize() / 2.0f;
    // glm::vec2 centeredTextCenter = imageCenter;
    // if (image_on_left_){
    //	centeredTextCenter += glm::vec2(icon_->GetSize().x / 2.0f +
    //horizontal_spacing_ + textSize.x / 2.0f, 0);
    // }
    // else {
    //	centeredTextCenter -= glm::vec2(icon_->GetSize().x / 2.0f +
    //horizontal_spacing_ + textSize.x / 2.0f, 0);
    // }
    // text_->GetText()->SetCenter(centeredTextCenter);
  }
}

glm::vec2 OptionUnit::GetPosition() const {
  // float posX = std::min(icon_->GetPosition().x, text_->GetPosition().x);
  // float posY = std::min(icon_->GetPosition().y, text_->GetPosition().y);

  // return glm::vec2(posX, posY);

  return image_on_left_ ? icon_->GetPosition() : text_->GetPosition();
}

void OptionUnit::SetHorizontalSpacing(float spacing) {
  horizontal_spacing_ = spacing;
}

float OptionUnit::GetHorizontalSpacing() const { return horizontal_spacing_; }

void OptionUnit::SetState(OptionState state) {
  state_ = state;
  switch (state_) {
    case OptionState::kClicked:
      // Set color to green
      icon_->SetColor(glm::vec4(colorMap.at(Color::Green), 1.0f));
      break;
    case OptionState::kHovered:
      // Set color to yellow
      icon_->SetColor(glm::vec4(colorMap.at(Color::Yellow), 1.0f));
      break;
    default:
      // Set color to white
      icon_->SetColor(glm::vec4(colorMap.at(Color::White), 1.0f));
      break;
  }
}

OptionState OptionUnit::GetState() const { return state_; }

void OptionUnit::SetImageOnLeft(bool imageOnLeft) {
  image_on_left_ = imageOnLeft;
}

bool OptionUnit::IsImageOnLeft() const { return image_on_left_; }

void OptionUnit::SetTextOnCenter(bool textOnCenter) {
  text_on_center_ = textOnCenter;
}

bool OptionUnit::IsTextOnCenter() const { return text_on_center_; }

bool OptionUnit::IsPositionInsideIcon(glm::vec2 position) const {
  // Assume the icon is a bubble
  glm::vec2 center = icon_->GetPosition() + icon_->GetSize() / 2.0f;
  float distance = glm::length(position - center);
  float radius = icon_->GetSize().x / 2.0f;
  return !areFloatsGreater(distance, radius);
}

void OptionUnit::Draw() {
  icon_->Draw();
  // if (text_on_center_) {
  //	text_->SetCentered(true);
  // }
  // else {
  //	text_->SetCentered(false);
  // }
  text_->Draw();
}
