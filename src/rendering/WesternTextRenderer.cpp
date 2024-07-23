/*
 * WesternTextRenderer.cpp
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

#include "WesternTextRenderer.h"

std::pair<glm::vec3, bool> WesternTextRenderer::GetTextSize(
    std::u32string text, float scale, float lineWidth, float lineSpacingFactor,
    float additionalPadding) {
  glm::vec2 finalSize = glm::vec2(0.f);

  // iterate through all characters
  std::u32string::const_iterator c;
  // Find the word's start (inclusive) and end (exlusive) index.
  std::pair<int, int> wordIndex = findWord(text, 0);

  // Memorize the initial x and y position.
  float x = 0.f, y = 0.f;
  float initialX = x;

  float lenOfLine = 0.f;

  // Space between lines
  float lineSpacing = characterMap[benchmarkChar][CharStyle::REGULAR].Size.y *
                          scale * lineSpacingFactor +
                      additionalPadding;

  //  A boolean value for whether the word is the first word of the line.
  bool firstWord = true;

  // char style
  CharStyle charStyle = CharStyle::REGULAR;

  // last character of the last word of the longest line.
  char32_t lastCharOfLongestLine = U'\n';

  bool hasDescendersInLastLine = false;

  while (wordIndex.second > wordIndex.first) {
    std::u32string word =
        text.substr(wordIndex.first, wordIndex.second - wordIndex.first);
    auto wordStyles = getEachCharacterStyle(word, charStyle);
    word = wordStyles.first;
    auto styles = wordStyles.second;
    charStyle = styles.back();

    // Get the x where the word ends.
    float endX = x;
    for (c = word.begin(); c != word.end(); ++c) {
      charStyle = styles[c - word.begin()];
      Character ch = characterMap.at(*c).at(charStyle);
      // If it is the last character of the word, then add the bearing.x and
      // size.x to the endX.
      if (c == std::prev(word.end())) {
        endX += (ch.Bearing.x + ch.Size.x) * scale;
      } else {
        // now advance cursors for next glyph
        endX += (ch.Advance >> 6) * scale;  // bitshift by 6 to get value in
                                            // pixels (1/64th times 2^6 = 64)
      }
    }

    if (lenOfLine + endX - x > lineWidth) {
      y += lineSpacing;
      hasDescendersInLastLine = false;
      x = initialX;
      finalSize.x = std::max(finalSize.x, lenOfLine);
      lenOfLine = 0.f;
      assert(!firstWord &&
             "The width of the line should at least have space for one word.");
      firstWord = true;
    } else {
      if (lenOfLine > finalSize.x || lastCharOfLongestLine == U'\n') {
        lastCharOfLongestLine = word.back();
      }
      firstWord = false;
    }

    // Move the end index of the word to the start of the next word.
    while (wordIndex.second < text.size() && text[wordIndex.second] == U' ') {
      ++wordIndex.second;
      word.append(U" ");
      styles.emplace_back(styles.back());
    }
    for (c = word.begin(); c != word.end(); ++c) {
      if (IsDescender(*c)) {
        hasDescendersInLastLine = true;
      }
      charStyle = styles[c - word.begin()];
      Character ch = characterMap.at(*c).at(charStyle);
      //// If it is the last character of the word, then add the bearing.x and
      /// size.x to the endX.
      // if (c == std::prev(word.end())) {
      //     x += (ch.Bearing.x + ch.Size.x) * scale;
      // }
      // else {
      //     // now advance cursors for next glyph
      //     x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in
      //     pixels (1/64th times 2^6 = 64)
      // }
      //  now advance cursors for next glyph
      x += (ch.Advance >> 6) * scale;  // bitshift by 6 to get value in pixels
                                       // (1/64th times 2^6 = 64)
      lenOfLine = x - initialX;
    }

    // Get the next word.
    wordIndex = findWord(text, wordIndex.second);
    // If it is the end of the line, move to the next line.
    while (wordIndex.second == wordIndex.first &&
           wordIndex.second < text.length() && text[wordIndex.first] == U'\n') {
      y += lineSpacing;
      hasDescendersInLastLine = false;
      x = initialX;
      finalSize.x = std::max(finalSize.x, lenOfLine);
      lenOfLine = 0.f;
      firstWord = true;
      ++wordIndex.second;
      while (wordIndex.second < text.length() &&
             text[wordIndex.second] == U' ') {
        ++wordIndex.second;
      }
      wordIndex = findWord(text, wordIndex.second);
    }
  }
  if (finalSize.x < lenOfLine) {
    finalSize.x = lenOfLine;
  }
  // float lastCharAdvance =
  // characterMap.at(CharStyle::REGULAR).at(lastCharOfLongestLine).Advance >> 6;
  // float lastCharBearing =
  // characterMap.at(CharStyle::REGULAR).at(lastCharOfLongestLine).Bearing.x;
  // float lastCharSize =
  // characterMap.at(CharStyle::REGULAR).at(lastCharOfLongestLine).Size.x;
  // finalSize.x -= (lastCharAdvance - lastCharBearing - lastCharSize) * scale;
  finalSize.y =
      y + characterMap[benchmarkChar][CharStyle::REGULAR].Size.y * scale;
  auto paragraphSize = glm::vec3(finalSize, lineSpacing);
  return std::make_pair(glm::vec3(finalSize, lineSpacing),
                        hasDescendersInLastLine);
}

std::pair<float, float> WesternTextRenderer::RenderText(
    std::u32string text, float x, float y, float scale, float lineWidth,
    float lineSpacingFactor, float additionalPadding, glm::vec3 color,
    float alpha) {
  // activate corresponding render state
  this->shader.Use();
  this->shader.SetVector3f("textColor", color);
  this->shader.SetFloat("alpha", alpha);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(this->VAO);

  // iterate through all characters
  std::u32string::const_iterator c;
  // Find the word's start (inclusive) and end (exlusive) index.
  std::pair<int, int> wordIndex = findWord(text, 0);

  // Memorize the initial x and y position.
  float initialX = x;

  float lenOfLine = 0.f;

  // Space between lines
  float lineSpacing = characterMap[benchmarkChar][CharStyle::REGULAR].Size.y *
                          scale * lineSpacingFactor +
                      additionalPadding;

  //  A boolean value for whether the word is the first word of the line.
  bool firstWord = true;

  // char style
  CharStyle charStyle = CharStyle::REGULAR;

  while (wordIndex.second > wordIndex.first) {
    std::u32string word =
        text.substr(wordIndex.first, wordIndex.second - wordIndex.first);
    auto wordStyles = getEachCharacterStyle(word, charStyle);
    word = wordStyles.first;
    auto styles = wordStyles.second;
    charStyle = styles.back();

    // Get the x where the word ends.
    float endX = x;
    for (c = word.begin(); c != word.end(); ++c) {
      charStyle = styles[c - word.begin()];
      Character ch = characterMap.at(*c).at(charStyle);
      // If it is the last character of the word, then add the bearing.x and
      // size.x to the endX.
      if (c == std::prev(word.end())) {
        endX += (ch.Bearing.x + ch.Size.x) * scale;
      } else {
        // now advance cursors for next glyph
        endX += (ch.Advance >> 6) * scale;  // bitshift by 6 to get value in
                                            // pixels (1/64th times 2^6 = 64)
      }
    }

    if (lenOfLine + endX - x > lineWidth) {
      y += lineSpacing;
      x = initialX;
      lenOfLine = 0.f;
      assert(!firstWord &&
             "The width of the line should at least have space for one word.");
      firstWord = true;
    } else {
      firstWord = false;
    }

    // Move the end index of the word to the start of the next word.
    while (wordIndex.second < text.size() && text[wordIndex.second] == U' ') {
      ++wordIndex.second;
      word.append(U" ");
      styles.emplace_back(styles.back());
    }

    for (c = word.begin(); c != word.end(); ++c) {
      charStyle = styles[c - word.begin()];
      Character ch = characterMap.at(*c).at(charStyle);

      float xpos = x + ch.Bearing.x * scale;
      float ypos =
          y + (this->characterMap.at(benchmarkChar).at(charStyle).Bearing.y -
               ch.Bearing.y) *
                  scale;
      float w = ch.Size.x * scale;
      float h = ch.Size.y * scale;
      // update VBO for each character
      float vertices[6][4] = {
          {xpos, ypos + h, 0.0f, 1.0f},     {xpos + w, ypos, 1.0f, 0.0f},
          {xpos, ypos, 0.0f, 0.0f},         {xpos, ypos + h, 0.0f, 1.0f},
          {xpos + w, ypos + h, 1.0f, 1.0f}, {xpos + w, ypos, 1.0f, 0.0f}};
      // render glyph texture over quad
      glBindTexture(GL_TEXTURE_2D, ch.TextureID);
      // update content of VBO memory
      glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
      glBufferSubData(
          GL_ARRAY_BUFFER, 0, sizeof(vertices),
          vertices);  // be sure to use glBufferSubData and not glBufferData
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      // render quad
      glDrawArrays(GL_TRIANGLES, 0, 6);
      // now advance cursors for next glyph
      x += (ch.Advance >> 6) * scale;  // bitshift by 6 to get value in pixels
                                       // (1/64th times 2^6 = 64)
      lenOfLine = x - initialX;
    }

    // Get the next word.
    wordIndex = findWord(text, wordIndex.second);
    // If it is the end of the line, move to the next line.
    while (wordIndex.second == wordIndex.first &&
           wordIndex.second < text.length() && text[wordIndex.first] == U'\n') {
      y += lineSpacing;
      x = initialX;
      lenOfLine = 0.f;
      firstWord = true;
      ++wordIndex.second;
      while (wordIndex.second < text.length() &&
             text[wordIndex.second] == U' ') {
        ++wordIndex.second;
      }
      wordIndex = findWord(text, wordIndex.second);
    }
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);

  return std::make_pair(
      y + characterMap[benchmarkChar][CharStyle::REGULAR].Size.y * scale,
      lineSpacing);
}

std::pair<float, float> WesternTextRenderer::RenderCenteredText(
    std::u32string text, float x, float y, float scale, float lineWidth,
    float lineSpacingFactor, float additionalPadding, glm::vec2 center,
    glm::vec3 color, float alpha) {
  // activate corresponding render state
  this->shader.Use();
  this->shader.SetVector3f("textColor", color);
  this->shader.SetFloat("alpha", alpha);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(this->VAO);

  // Has cursor at the end of the text.
  bool hasCursor = !text.empty() && text.back() == U'|';
  if (hasCursor) {
    text.pop_back();
  }

  auto textSizeInfo =
      GetTextSize(text, scale, lineWidth, lineSpacingFactor, additionalPadding);

  float textSizeY = textSizeInfo.first.y;

  // iterate through all characters
  std::u32string::const_iterator c;
  // Find the word's start (inclusive) and end (exlusive) index.
  std::pair<int, int> wordIndex = findWord(text, 0);

  // Memorize the initial x and y position.
  float initialX = x;

  float lenOfLine = 0.f;

  // Space between lines
  float lineSpacing = characterMap[benchmarkChar][CharStyle::REGULAR].Size.y *
                          scale * lineSpacingFactor +
                      additionalPadding;

  //  A boolean value for whether the word is the first word of the line.
  bool firstWord = true;

  // char style
  CharStyle charStyle = CharStyle::REGULAR;

  size_t numOfSpacesAtLineEnd = 0;

  std::vector<Character> characters;
  std::vector<float> xpositions, ypositions, widths, heights;

  //// Assume the text contains only one line.
  // glm::vec2 offset = glm::vec2(
  //     0.f,
  //     center.y - (y + characterMap[benchmarkChar][CharStyle::REGULAR].Size.y
  //     *
  //                         scale * 0.5f));

  // Consider both one-line and multi-line text.
  glm::vec2 offset = glm::vec2(0.f, center.y - (y + textSizeY * 0.5f));

  while (wordIndex.second > wordIndex.first) {
    std::u32string word =
        text.substr(wordIndex.first, wordIndex.second - wordIndex.first);
    auto wordStyles = getEachCharacterStyle(word, charStyle);
    word = wordStyles.first;
    auto styles = wordStyles.second;
    charStyle = styles.back();

    // Get the x where the word ends.
    float endX = x;
    for (c = word.begin(); c != word.end(); ++c) {
      charStyle = styles[c - word.begin()];
      Character ch = characterMap.at(*c).at(charStyle);
      // If it is the last character of the word, then add the bearing.x and
      // size.x to the endX.
      if (c == std::prev(word.end())) {
        endX += (ch.Bearing.x + ch.Size.x) * scale;
      } else {
        // now advance cursors for next glyph
        endX += (ch.Advance >> 6) * scale;  // bitshift by 6 to get value in
                                            // pixels (1/64th times 2^6 = 64)
      }
    }

    if (lenOfLine + endX - x > lineWidth) {
      // Caculate the width of spaces at the end of the line.
      if (numOfSpacesAtLineEnd > 0) {
        float spacesWidthAtLineEnd =
            characterMap.at(U'0').at(CharStyle::REGULAR).Size.x * 0.5f * scale *
            numOfSpacesAtLineEnd;
        lenOfLine -= spacesWidthAtLineEnd;
      }
      offset.x = center.x - (initialX + lenOfLine * 0.5f);
      RenderLine(characters, xpositions, ypositions, widths, heights, offset);
      y += lineSpacing;
      x = initialX;
      lenOfLine = 0.f;
      assert(!firstWord &&
             "The width of the line should at least have space for one word.");
      firstWord = true;
    } else {
      firstWord = false;
    }
    numOfSpacesAtLineEnd = 0;

    // Move the end index of the word to the start of the next word.
    while (wordIndex.second < text.size() && text[wordIndex.second] == U' ') {
      ++wordIndex.second;
      word.append(U" ");
      styles.emplace_back(styles.back());
      ++numOfSpacesAtLineEnd;
    }

    for (c = word.begin(); c != word.end(); ++c) {
      charStyle = styles[c - word.begin()];
      Character ch = characterMap.at(*c).at(charStyle);

      float xpos = x + ch.Bearing.x * scale;
      float ypos =
          y + (this->characterMap.at(benchmarkChar).at(charStyle).Bearing.y -
               ch.Bearing.y) *
                  scale;
      float w = ch.Size.x * scale;
      float h = ch.Size.y * scale;
      // store the position, width, and height of the character.
      characters.emplace_back(ch);
      xpositions.emplace_back(xpos);
      ypositions.emplace_back(ypos);
      widths.emplace_back(w);
      heights.emplace_back(h);
      //// update VBO for each character
      // float vertices[6][4] = {
      //     { xpos,     ypos + h,   0.0f, 1.0f },
      //     { xpos + w, ypos,       1.0f, 0.0f },
      //     { xpos,     ypos,       0.0f, 0.0f },

      //    { xpos,     ypos + h,   0.0f, 1.0f },
      //    { xpos + w, ypos + h,   1.0f, 1.0f },
      //    { xpos + w, ypos,       1.0f, 0.0f }
      //};
      //// render glyph texture over quad
      // glBindTexture(GL_TEXTURE_2D, ch.TextureID);
      //// update content of VBO memory
      // glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
      // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be
      // sure to use glBufferSubData and not glBufferData
      // glBindBuffer(GL_ARRAY_BUFFER, 0);
      //// render quad
      // glDrawArrays(GL_TRIANGLES, 0, 6);
      //  now advance cursors for next glyph
      x += (ch.Advance >> 6) * scale;  // bitshift by 6 to get value in pixels
                                       // (1/64th times 2^6 = 64)
      lenOfLine = x - initialX;
    }

    // Get the next word.
    wordIndex = findWord(text, wordIndex.second);
    // If it is the end of the line, move to the next line.
    while (wordIndex.second == wordIndex.first &&
           wordIndex.second < text.length() && text[wordIndex.first] == U'\n') {
      offset.x = center.x - (initialX + lenOfLine * 0.5f);
      RenderLine(characters, xpositions, ypositions, widths, heights, offset);
      y += lineSpacing;
      x = initialX;
      lenOfLine = 0.f;
      firstWord = true;
      ++wordIndex.second;
      while (wordIndex.second < text.length() &&
             text[wordIndex.second] == U' ') {
        ++wordIndex.second;
      }
      wordIndex = findWord(text, wordIndex.second);
    }
  }

  // Append the cursor character at the end if it is needed.
  if (hasCursor) {
    Character ch = characterMap.at(U'|').at(CharStyle::REGULAR);
    float xpos = x + ch.Bearing.x * scale;
    float ypos =
        y +
        (this->characterMap.at(benchmarkChar).at(CharStyle::REGULAR).Bearing.y -
         ch.Bearing.y) *
            scale;
    xpositions.emplace_back(xpos);
    ypositions.emplace_back(ypos);
    widths.emplace_back(ch.Size.x * scale);
    heights.emplace_back(ch.Size.y * scale);
    // offset.x = center.x - (initialX + lenOfLine * 0.5f);
    characters.emplace_back(ch);
  }
  offset.x = center.x - (initialX + lenOfLine * 0.5f);
  RenderLine(characters, xpositions, ypositions, widths, heights, offset);

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);

  return std::make_pair(
      y + characterMap[benchmarkChar][CharStyle::REGULAR].Size.y * scale,
      lineSpacing);
}

std::pair<float, float> WesternTextRenderer::RenderRightAlignedText(
    std::u32string text, float x, float y, float scale, float lineWidth,
    float lineSpacingFactor, float additionalPadding, glm::vec3 color,
    float alpha) {
  // activate corresponding render state
  this->shader.Use();
  this->shader.SetVector3f("textColor", color);
  this->shader.SetFloat("alpha", alpha);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(this->VAO);

  // iterate through all characters
  std::u32string::const_iterator c;
  // Find the word's start (inclusive) and end (exlusive) index.
  std::pair<int, int> wordIndex = findWord(text, 0);

  // Memorize the initial x and y position.
  float initialX = x;

  float lenOfLine = 0.f;

  // Space between lines
  float lineSpacing = characterMap[benchmarkChar][CharStyle::REGULAR].Size.y *
                          scale * lineSpacingFactor +
                      additionalPadding;

  //  A boolean value for whether the word is the first word of the line.
  bool firstWord = true;

  // char style
  CharStyle charStyle = CharStyle::REGULAR;

  std::vector<Character> characters;
  std::vector<float> xpositions, ypositions, widths, heights;

  while (wordIndex.second > wordIndex.first) {
    std::u32string word =
        text.substr(wordIndex.first, wordIndex.second - wordIndex.first);
    auto wordStyles = getEachCharacterStyle(word, charStyle);
    word = wordStyles.first;
    auto styles = wordStyles.second;
    charStyle = styles.back();

    // Get the x where the word ends.
    float endX = x;
    for (c = word.begin(); c != word.end(); ++c) {
      charStyle = styles[c - word.begin()];
      Character ch = characterMap.at(*c).at(charStyle);
      // If it is the last character of the word, then add the bearing.x and
      // size.x to the endX.
      if (c == std::prev(word.end())) {
        endX += (ch.Bearing.x + ch.Size.x) * scale;
      } else {
        // now advance cursors for next glyph
        endX += (ch.Advance >> 6) * scale;  // bitshift by 6 to get value in
                                            // pixels (1/64th times 2^6 = 64)
      }
    }

    if (lenOfLine + endX - x > lineWidth) {
      RenderLine(characters, xpositions, ypositions, widths, heights,
                 glm::vec2(-lenOfLine, 0.f));
      y += lineSpacing;
      x = initialX;
      lenOfLine = 0.f;
      assert(!firstWord &&
             "The width of the line should at least have space for one word.");
      firstWord = true;
    } else {
      firstWord = false;
    }

    // Move the end index of the word to the start of the next word.
    while (wordIndex.second < text.size() && text[wordIndex.second] == U' ') {
      ++wordIndex.second;
      word.append(U" ");
      styles.emplace_back(styles.back());
    }

    for (c = word.begin(); c != word.end(); ++c) {
      charStyle = styles[c - word.begin()];
      Character ch = characterMap.at(*c).at(charStyle);

      float xpos = x + ch.Bearing.x * scale;
      float ypos =
          y + (this->characterMap.at(benchmarkChar).at(charStyle).Bearing.y -
               ch.Bearing.y) *
                  scale;
      float w = ch.Size.x * scale;
      float h = ch.Size.y * scale;
      // store the position, width, and height of the character.
      characters.emplace_back(ch);
      xpositions.emplace_back(xpos);
      ypositions.emplace_back(ypos);
      widths.emplace_back(w);
      heights.emplace_back(h);
      //// update VBO for each character
      // float vertices[6][4] = {
      //     {xpos, ypos + h, 0.0f, 1.0f},     {xpos + w, ypos, 1.0f, 0.0f},
      //     {xpos, ypos, 0.0f, 0.0f},         {xpos, ypos + h, 0.0f, 1.0f},
      //     {xpos + w, ypos + h, 1.0f, 1.0f}, {xpos + w, ypos, 1.0f, 0.0f}};
      //// render glyph texture over quad
      // glBindTexture(GL_TEXTURE_2D, ch.TextureID);
      //// update content of VBO memory
      // glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
      // glBufferSubData(
      //     GL_ARRAY_BUFFER, 0, sizeof(vertices),
      //     vertices);  // be sure to use glBufferSubData and not glBufferData
      // glBindBuffer(GL_ARRAY_BUFFER, 0);
      //// render quad
      // glDrawArrays(GL_TRIANGLES, 0, 6);
      //  now advance cursors for next glyph
      x += (ch.Advance >> 6) * scale;  // bitshift by 6 to get value in pixels
                                       // (1/64th times 2^6 = 64)
      lenOfLine = x - initialX;
    }

    // Get the next word.
    wordIndex = findWord(text, wordIndex.second);
    // If it is the end of the line, move to the next line.
    while (wordIndex.second == wordIndex.first &&
           wordIndex.second < text.length() && text[wordIndex.first] == U'\n') {
      RenderLine(characters, xpositions, ypositions, widths, heights,
                 glm::vec2(-lenOfLine, 0.f));
      y += lineSpacing;
      x = initialX;
      lenOfLine = 0.f;
      firstWord = true;
      ++wordIndex.second;
      while (wordIndex.second < text.length() &&
             text[wordIndex.second] == U' ') {
        ++wordIndex.second;
      }
      wordIndex = findWord(text, wordIndex.second);
    }
  }
  RenderLine(characters, xpositions, ypositions, widths, heights,
             glm::vec2(-lenOfLine, 0.f));
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);

  return std::make_pair(
      y + characterMap[benchmarkChar][CharStyle::REGULAR].Size.y * scale,
      lineSpacing);
}