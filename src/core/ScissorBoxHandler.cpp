#include <iostream>

#include "ScissorBoxHandler.h"

ScissorBoxHandler::ScissorBoxHandler()
    : scissor_box_({0, 0, INT_MAX, INT_MAX}),
      prev_scissor_box_({0, 0, INT_MAX, INT_MAX}),
      scissor_test_enabled_(false) {}

ScissorBoxHandler::~ScissorBoxHandler() {}

void ScissorBoxHandler::SetScissorBox(GLint x, GLint y, GLsizei width,
                                      GLsizei height) {
  MemorizeScissorBox();
  scissor_box_ = {x, y, width, height};
  glScissor(x, y, width, height);
}

void ScissorBoxHandler::SetScissorBox(const ScissorBox& scissorBox) {
  SetScissorBox(scissorBox.x, scissorBox.y, scissorBox.width,
                scissorBox.height);
}

void ScissorBoxHandler::SetIntersectedScissorBox(GLint x, GLint y,
                                                 GLsizei width,
                                                 GLsizei height) {
  ScissorBox intersection;
  intersection.x = std::max(scissor_box_.x, x);
  intersection.y = std::max(scissor_box_.y, y);
  intersection.width =
      std::min(scissor_box_.x + scissor_box_.width, x + width) - intersection.x;
  intersection.height =
      std::min(scissor_box_.y + scissor_box_.height, y + height) -
      intersection.y;
  intersection.width = std::max(0, intersection.width);
  intersection.height = std::max(0, intersection.height);
  SetScissorBox(intersection);
}
void ScissorBoxHandler::SetIntersectedScissorBox(const ScissorBox& scissorBox) {
  SetIntersectedScissorBox(scissorBox.x, scissorBox.y, scissorBox.width,
                           scissorBox.height);
}

ScissorBoxHandler::ScissorBox ScissorBoxHandler::GetScissorBox() {
  return scissor_box_;
}

ScissorBoxHandler::ScissorBox ScissorBoxHandler::GetPrevScissorBox() {
  return prev_scissor_box_;
}

void ScissorBoxHandler::EnableScissorTest() {
  if (!scissor_test_enabled_) {
    glEnable(GL_SCISSOR_TEST);
    scissor_test_enabled_ = true;
  }
}

void ScissorBoxHandler::DisableScissorTest() {
  if (scissor_test_enabled_) {
    glDisable(GL_SCISSOR_TEST);
    scissor_test_enabled_ = false;
  }
}

void ScissorBoxHandler::RestoreScissorBox() {
  SetScissorBox(prev_scissor_box_);
}

bool ScissorBoxHandler::IsScissorTestEnabled() const {
  return scissor_test_enabled_;
}

void ScissorBoxHandler::MemorizeScissorBox() {
  // GLint prevScissorBox[4];
  // glGetIntegerv(GL_SCISSOR_BOX, prevScissorBox);
  // prev_scissor_box_ = { prevScissorBox[0], prevScissorBox[1],
  // prevScissorBox[2], prevScissorBox[3] };
  prev_scissor_box_ = scissor_box_;
}
