#include "CJKTextRenderer.h"

std::pair<glm::vec3, bool> CJKTextRenderer::GetTextSize(
    std::u32string text, float scale, float lineWidth, float lineSpacingFactor,
    float additionalPadding) {
  glm::vec2 finalSize = glm::vec2(0.f);

  // iterate through all characters
  std::u32string::const_iterator c;

  // Memorize the initial x and y position.
  float x = 0.f, y = 0.f;
  float initialX = x;

  float lenOfLine = 0.f;

  // Space between lines
  float lineSpacing = characterMap[benchmarkChar][CharStyle::REGULAR].Size.y *
                          scale * lineSpacingFactor +
                      additionalPadding;

  //  A boolean value for whether the word is the first word of the line. For
  //  CJK characters, each character can be seen as a word.
  bool firstWord = true;

  // char style
  CharStyle charStyle = CharStyle::REGULAR;

  // last character of the last word of the longest line.
  char32_t lastCharOfLongestLine = U'\n';

  bool hasDescendersInLastLine = false;

  std::u32string word;

  for (size_t i = 0; i < text.size(); ++i) {
    // For CJK characters, we don't need to check if it is a space, each
    // character can be seen as a word. When it is '{', '}', '[', ']', '\t', we
    // would continue to find the next character.
    if (text[i] == U'{' || text[i] == U'}' || text[i] == U'[' ||
        text[i] == U']') {
      word.push_back(text[i]);
      continue;
    } else if (text[i] == U'\n') {
      y += lineSpacing;
      hasDescendersInLastLine = false;
      x = initialX;
      finalSize.x = std::max(finalSize.x, lenOfLine);
      lenOfLine = 0.f;
      firstWord = true;
      continue;
    }
    word.push_back(text[i]);
    auto wordStyles = getEachCharacterStyle(word, charStyle);
    word = wordStyles.first;
    auto styles = wordStyles.second;

    assert((word.size() == 1 || word.size() == tabSize) &&
           styles.size() == word.size() + 1 &&
           "The word should only contain one character.");
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
    while (i + 1 < text.size() && text[i + 1] == U' ') {
      ++i;
      word.append(U" ");
      styles.emplace_back(styles.back());
    }
    for (c = word.begin(); c != word.end(); ++c) {
      if (IsDescender(*c)) {
        hasDescendersInLastLine = true;
      }
      charStyle = styles[c - word.begin()];
      Character ch = characterMap.at(*c).at(charStyle);

      //  now advance cursors for next glyph
      x += (ch.Advance >> 6) * scale;  // bitshift by 6 to get value in pixels
                                       // (1/64th times 2^6 = 64)
      lenOfLine = x - initialX;
    }
    word.clear();
  }

  if (finalSize.x < lenOfLine) {
    finalSize.x = lenOfLine;
  }

  finalSize.y =
      y + characterMap[benchmarkChar][CharStyle::REGULAR].Size.y * scale;
  return std::make_pair(glm::vec3(finalSize, lineSpacing),
                        hasDescendersInLastLine);
}

std::pair<float, float> CJKTextRenderer::RenderText(
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

  // Memorize the initial x and y position.
  float initialX = x;

  float lenOfLine = 0.f;

  // Space between lines
  float lineSpacing = characterMap[benchmarkChar][CharStyle::REGULAR].Size.y *
                          scale * lineSpacingFactor +
                      additionalPadding;

  //  A boolean value for whether the word is the first word of the line. For
  //  CJK characters, each character can be seen as a word.
  bool firstWord = true;

  // char style
  CharStyle charStyle = CharStyle::REGULAR;

  std::u32string word;

  for (size_t i = 0; i < text.size(); ++i) {
    // For CJK characters, we don't need to check if it is a space, each
    // character can be seen as a word. When it is '{', '}', '[', ']', '\t', we
    // would continue to find the next character.
    if (text[i] == U'{' || text[i] == U'}' || text[i] == U'[' ||
        text[i] == U']') {
      word.push_back(text[i]);
      continue;
    } else if (text[i] == U'\n') {
      y += lineSpacing;
      x = initialX;
      lenOfLine = 0.f;
      firstWord = true;
      continue;
    }
    word.push_back(text[i]);
    auto wordStyles = getEachCharacterStyle(word, charStyle);
    word = wordStyles.first;
    auto styles = wordStyles.second;
    assert((word.size() == 1 || word.size() == tabSize) &&
           styles.size() == word.size() + 1 &&
           "The word should only contain one character.");
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
    while (i + 1 < text.size() && text[i + 1] == U' ') {
      ++i;
      word.append(U" ");
      styles.emplace_back(styles.back());
    }
    /*char32_t charOfLargeBearingY = benchmarkChar;*/
    for (c = word.begin(); c != word.end(); ++c) {
      charStyle = styles[c - word.begin()];
      Character ch = characterMap.at(*c).at(charStyle);

      float xpos = x + ch.Bearing.x * scale;
      // float benchMarkBearingY =
      //     this->characterMap.at(benchmarkChar).at(charStyle).Bearing.y;
      // float curCharBearingY = ch.Bearing.y;
      // if (curCharBearingY > benchMarkBearingY) {
      //   // print the hex string of the current char:
      //   if (*c != 0xFF09 && *c != 0xFF08) {
      //       std::cout << "larger breaing Y
      //       apprear!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" <<std::endl;
      //       charOfLargeBearingY = *c;
      //     }
      // }
      float ypos =
          y + (this->characterMap.at(benchmarkChar).at(charStyle).Bearing.y -
               ch.Bearing.y) *
                  scale;

      float w = ch.Size.x * scale;
      float h = ch.Size.y * scale;
      // update VBO for each character
      float vertices[6][4] = {
          {xpos, ypos + h, 0.0f, 1.0f}, {xpos + w, ypos, 1.0f, 0.0f},
          {xpos, ypos, 0.0f, 0.0f},

          {xpos, ypos + h, 0.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 1.0f},
          {xpos + w, ypos, 1.0f, 0.0f}};
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
    word.clear();
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);

  return std::make_pair(
      y + characterMap[benchmarkChar][CharStyle::REGULAR].Size.y * scale,
      lineSpacing);
}

std::pair<float, float> CJKTextRenderer::RenderCenteredText(
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

  // iterate through all characters
  std::u32string::const_iterator c;

  // Memorize the initial x and y position.
  float initialX = x;

  float lenOfLine = 0.f;

  // Space between lines
  float lineSpacing = characterMap[benchmarkChar][CharStyle::REGULAR].Size.y *
                          scale * lineSpacingFactor +
                      additionalPadding;

  //  A boolean value for whether the word is the first word of the line. For
  //  CJK characters, each character can be seen as a word.
  bool firstWord = true;

  // char style
  CharStyle charStyle = CharStyle::REGULAR;

  std::u32string word;

  std::vector<Character> characters;
  std::vector<float> xpositions, ypositions, widths, heights;

  // Assume the text contains only one line.
  glm::vec2 offset = glm::vec2(
      0.f,
      center.y - (y + characterMap[benchmarkChar][CharStyle::REGULAR].Size.y *
                          scale * 0.5f));

  for (size_t i = 0; i < text.size(); ++i) {
    // For CJK characters, we don't need to check if it is a space, each
    // character can be seen as a word. When it is '{', '}', '[', ']', '\t', we
    // would continue to find the next character.
    if (text[i] == U'{' || text[i] == U'}' || text[i] == U'[' ||
        text[i] == U']') {
      word.push_back(text[i]);
      continue;
    } else if (text[i] == U'\n') {
      offset.x = center.x - (initialX + lenOfLine * 0.5f);
      RenderLine(characters, xpositions, ypositions, widths, heights, offset);
      y += lineSpacing;
      x = initialX;
      lenOfLine = 0.f;
      firstWord = true;
      continue;
    }
    word.push_back(text[i]);
    auto wordStyles = getEachCharacterStyle(word, charStyle);
    word = wordStyles.first;
    auto styles = wordStyles.second;
    assert((word.size() == 1 || word.size() == tabSize) &&
           styles.size() == word.size() + 1 &&
           "The word should only contain one character.");
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

    // Move the end index of the word to the start of the next word.
    while (i + 1 < text.size() && text[i + 1] == U' ') {
      ++i;
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
      //  now advance cursors for next glyph
      x += (ch.Advance >> 6) * scale;  // bitshift by 6 to get value in pixels
                                       // (1/64th times 2^6 = 64)
      lenOfLine = x - initialX;
    }
    word.clear();
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

std::pair<float, float> CJKTextRenderer::RenderRightAlignedText(
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

  // Memorize the initial x and y position.
  float initialX = x;

  float lenOfLine = 0.f;

  // Space between lines
  float lineSpacing = characterMap[benchmarkChar][CharStyle::REGULAR].Size.y *
                          scale * lineSpacingFactor +
                      additionalPadding;

  //  A boolean value for whether the word is the first word of the line. For
  //  CJK characters, each character can be seen as a word.
  bool firstWord = true;

  // char style
  CharStyle charStyle = CharStyle::REGULAR;

  std::u32string word;

  std::vector<Character> characters;
  std::vector<float> xpositions, ypositions, widths, heights;

  for (size_t i = 0; i < text.size(); ++i) {
    // For CJK characters, we don't need to check if it is a space, each
    // character can be seen as a word. When it is '{', '}', '[', ']', '\t', we
    // would continue to find the next character.
    if (text[i] == U'{' || text[i] == U'}' || text[i] == U'[' ||
        text[i] == U']') {
      word.push_back(text[i]);
      continue;
    } else if (text[i] == U'\n') {
      RenderLine(characters, xpositions, ypositions, widths, heights,
                 glm::vec2(-lenOfLine, 0.f));
      y += lineSpacing;
      x = initialX;
      lenOfLine = 0.f;
      firstWord = true;
      continue;
    }
    word.push_back(text[i]);
    auto wordStyles = getEachCharacterStyle(word, charStyle);
    word = wordStyles.first;
    auto styles = wordStyles.second;
    assert((word.size() == 1 || word.size() == tabSize) &&
           styles.size() == word.size() + 1 &&
           "The word should only contain one character.");
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
    while (i + 1 < text.size() && text[i + 1] == U' ') {
      ++i;
      word.append(U" ");
      styles.emplace_back(styles.back());
    }
    /*char32_t charOfLargeBearingY = benchmarkChar;*/
    for (c = word.begin(); c != word.end(); ++c) {
      charStyle = styles[c - word.begin()];
      Character ch = characterMap.at(*c).at(charStyle);

      float xpos = x + ch.Bearing.x * scale;
      // float benchMarkBearingY =
      //     this->characterMap.at(benchmarkChar).at(charStyle).Bearing.y;
      // float curCharBearingY = ch.Bearing.y;
      // if (curCharBearingY > benchMarkBearingY) {
      //   // print the hex string of the current char:
      //   if (*c != 0xFF09 && *c != 0xFF08) {
      //       std::cout << "larger breaing Y
      //       apprear!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" <<std::endl;
      //       charOfLargeBearingY = *c;
      //     }
      // }
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
      // now advance cursors for next glyph
      x += (ch.Advance >> 6) * scale;  // bitshift by 6 to get value in pixels
                                       // (1/64th times 2^6 = 64)
      lenOfLine = x - initialX;
    }
    word.clear();
  }
  RenderLine(characters, xpositions, ypositions, widths, heights,
             glm::vec2(-lenOfLine, 0.f));
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);

  return std::make_pair(
      y + characterMap[benchmarkChar][CharStyle::REGULAR].Size.y * scale,
      lineSpacing);
}
