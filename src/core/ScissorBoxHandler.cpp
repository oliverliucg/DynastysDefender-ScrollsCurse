/*
 * ScissorBoxHandler.cpp
 * Copyright (C) 2024 Oliver Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "ScissorBoxHandler.h"

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
