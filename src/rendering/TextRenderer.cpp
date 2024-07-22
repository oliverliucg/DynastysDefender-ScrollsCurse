/*
 * TextRenderer.cpp
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

#include "TextRenderer.h"

std::unordered_map<char32_t, std::unordered_map<CharStyle, Character>>
    TextRenderer::characterMap;

std::unordered_map<char32_t, int> TextRenderer::characterCount;

void TextRenderer::Load(const std::string& font, unsigned int fontSize,
                        CharStyle charStyle,
                        const std::vector<FT_ULong> charactersToLoad) {
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    std::cerr << "ERROR::FREETYPE: Could not init FreeType Library"
              << std::endl;
    return;
  }
  // load font as face
  FT_Face face;
  if (FT_New_Face(ft, font.c_str(), 0, &face)) {
    std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
    FT_Done_FreeType(ft);
    return;
  }
  // set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, fontSize);
  // disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // pre - load / compile characters and store them
  for (const auto& c : charactersToLoad) {
    assert(TextRenderer::characterMap.find(c) ==
               TextRenderer::characterMap.end() ||
           TextRenderer::characterMap.at(c).find(charStyle) ==
                   TextRenderer::characterMap.at(c).end() &&
               "One character should only be loaded once under the current "
               "style.");

    // load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cerr << "ERROR::FREETYPE: Failed to load Glyph for character code "
                << std::hex << c << std::endl;
      continue;
    }
    // generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // now store character for later use
    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        face->glyph->advance.x};
    characterMap[c][charStyle] = character;
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  // destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

void TextRenderer::Load(const std::u32string& charactersToLoad) {
  ConfigManager& config = ConfigManager::GetInstance();
  // store the start hex of the font, the font path, and the characters to load
  std::unordered_map<char32_t, std::pair<std::string, std::vector<FT_ULong>>>
      regularCharacters;
  std::unordered_map<char32_t, std::pair<std::string, std::vector<FT_ULong>>>
      boldCharacters;
  std::unordered_map<char32_t, std::pair<std::string, std::vector<FT_ULong>>>
      italicCharacters;
  std::unordered_map<char32_t, std::pair<std::string, std::vector<FT_ULong>>>
      boldItalicCharacters;
  for (char32_t c : charactersToLoad) {
    if (isControlChar(c)) {
      continue;
    }
    if (characterMap.count(c) > 0) {
      assert(characterCount.find(c) != characterCount.end() &&
             characterCount.at(c) > 0 &&
             "The count of the character should be greater than 0 when it "
             "has been loaded.");
      ++characterCount[c];
      continue;
    }

    if (++characterCount[c] > 1) {
      // continue to the next character as one chacter only needs to be loaded
      // once.
      continue;
    }

    auto [fontStartHexRegular, fontPathRegular] =
        config.GetFontFilePath(c, CharStyle::REGULAR);
    auto [fontStartHexBold, fontPathBold] =
        config.GetFontFilePath(c, CharStyle::BOLD);
    auto [fontStartHexItalic, fontPathItalic] =
        config.GetFontFilePath(c, CharStyle::ITALIC);
    auto [fontStartHexBoldItalic, fontPathBoldItalic] =
        config.GetFontFilePath(c, CharStyle::BOLD_ITALIC);
    if (fontPathRegular.empty()) {
      assert(fontPathBold.empty() && fontPathItalic.empty() &&
             fontPathBoldItalic.empty());
      continue;
    }

    if (regularCharacters.count(fontStartHexRegular) == 0) {
      regularCharacters[fontStartHexRegular] = {fontPathRegular, {}};
    }
    if (boldCharacters.count(fontStartHexBold) == 0) {
      boldCharacters[fontStartHexBold] = {fontPathBold, {}};
    }
    if (italicCharacters.count(fontStartHexItalic) == 0) {
      italicCharacters[fontStartHexItalic] = {fontPathItalic, {}};
    }
    if (boldItalicCharacters.count(fontStartHexBoldItalic) == 0) {
      boldItalicCharacters[fontStartHexBoldItalic] = {fontPathBoldItalic, {}};
    }
    regularCharacters[fontStartHexRegular].second.emplace_back(c);
    boldCharacters[fontStartHexBold].second.emplace_back(c);
    italicCharacters[fontStartHexItalic].second.emplace_back(c);
    boldItalicCharacters[fontStartHexBoldItalic].second.emplace_back(c);
  }

  // load the characters
  for (const auto& [fontStartHex, fontCharacters] : regularCharacters) {
    Load(fontCharacters.first, kFontSize, CharStyle::REGULAR,
         fontCharacters.second);
  }
  for (const auto& [fontStartHex, fontCharacters] : boldCharacters) {
    Load(fontCharacters.first, kFontSize, CharStyle::BOLD,
         fontCharacters.second);
  }
  for (const auto& [fontStartHex, fontCharacters] : italicCharacters) {
    Load(fontCharacters.first, kFontSize, CharStyle::ITALIC,
         fontCharacters.second);
  }
  for (const auto& [fontStartHex, fontCharacters] : boldItalicCharacters) {
    Load(fontCharacters.first, kFontSize, CharStyle::BOLD_ITALIC,
         fontCharacters.second);
  }
}

void TextRenderer::UnLoadIfNotUsed(char32_t character) {
  if (isControlChar(character)) {
    assert(characterCount.find(character) == characterCount.end() &&
           characterMap.find(character) == characterMap.end() &&
           "Control character is not supposed to be loaded.");
    return;
  }
  assert(characterCount.at(character) > 0 &&
         characterMap.count(character) > 0 &&
         "Character is supposed to be loaded.");
  if (--characterCount[character] == 0) {
    characterCount.erase(character);
    for (auto& styleChar : characterMap.at(character)) {
      // Delete the texture.
      glDeleteTextures(1, &styleChar.second.TextureID);
    }
    characterMap.erase(character);
  }
}

void TextRenderer::UnLoadIfNotUsed(const std::u32string& charactersToLoad) {
  std::vector<unsigned int> texturesToDelete;
  for (const auto& character : charactersToLoad) {
    if (isControlChar(character)) {
      continue;
    }
    if (characterCount.find(character) == characterCount.end()) {
      assert(characterMap.find(character) == characterMap.end() &&
             "Control character is not supposed to be loaded.");
      continue;
    }
    if (--characterCount[character] == 0) {
      characterCount.erase(character);
      for (auto& styleChar : characterMap.at(character)) {
        texturesToDelete.emplace_back(styleChar.second.TextureID);
      }
      characterMap.erase(character);
    }
  }
  if (!texturesToDelete.empty()) {
    // Delete the textures.
    glDeleteTextures(texturesToDelete.size(), texturesToDelete.data());
  }
}

TextRenderer::TextRenderer(const Shader& shader, unsigned int width,
                           unsigned int height, char32_t benchmarkChar)
    : Renderer(shader), benchmarkChar(benchmarkChar) {
  // load and configure shader
  this->shader.SetMatrix4("projection",
                          glm::ortho(0.0f, static_cast<float>(width),
                                     static_cast<float>(height), 0.0f),
                          true);
  this->shader.SetInteger("text", 0);
  // configure VAO/VBO for texture quads
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);
  glBindVertexArray(this->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

std::u32string TextRenderer::replaceTabs(const std::u32string& text) {
  std::u32string result;
  for (const auto& c : text) {
    if (c == U'\t') {
      result.append(tabSize, U' ');
    } else {
      result.append(1, c);
    }
  }
  return result;
}

std::pair<std::u32string, std::vector<CharStyle>>
TextRenderer::getEachCharacterStyle(const std::u32string& text,
                                    CharStyle initialStyle) {
  std::u32string tmp = replaceTabs(text);
  std::vector<CharStyle> resultStyle;
  std::u32string resultText;
  CharStyle currentStyle = initialStyle;
  for (const auto& c : tmp) {
    if (c == U'{') {
      if (currentStyle == CharStyle::ITALIC) {
        currentStyle = CharStyle::BOLD_ITALIC;
      } else {
        currentStyle = CharStyle::BOLD;
      }
    } else if (c == U'[') {
      if (currentStyle == CharStyle::BOLD) {
        currentStyle = CharStyle::BOLD_ITALIC;
      } else {
        currentStyle = CharStyle::ITALIC;
      }
    } else if (c == U'}') {
      if (currentStyle == CharStyle::BOLD_ITALIC) {
        currentStyle = CharStyle::ITALIC;
      } else {
        currentStyle = CharStyle::REGULAR;
      }
    } else if (c == U']') {
      if (currentStyle == CharStyle::BOLD_ITALIC) {
        currentStyle = CharStyle::BOLD;
      } else {
        currentStyle = CharStyle::REGULAR;
      }
    } else {
      resultStyle.emplace_back(currentStyle);
      resultText.append(1, c);
    }
  }

  // push a char style at the end of the resultStyle to indicate the initial
  // style of the next word.
  resultStyle.emplace_back(currentStyle);

  return std::make_pair(resultText, resultStyle);
}

void TextRenderer::RenderChar(Character ch, float x, float y, float w,
                              float h) {
  // update VBO for each character
  float vertices[6][4] = {{x, y + h, 0.0f, 1.0f}, {x + w, y, 1.0f, 0.0f},
                          {x, y, 0.0f, 0.0f},

                          {x, y + h, 0.0f, 1.0f}, {x + w, y + h, 1.0f, 1.0f},
                          {x + w, y, 1.0f, 0.0f}};
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
}

void TextRenderer::RenderLine(std::vector<Character>& line,
                              std::vector<float>& xpositions,
                              std::vector<float>& ypositions,
                              std::vector<float>& widths,
                              std::vector<float>& heights, glm::vec2 offset) {
  for (size_t i = 0; i < line.size(); ++i) {
    RenderChar(line[i], xpositions[i] + offset.x, ypositions[i] + offset.y,
               widths[i], heights[i]);
  }
  line.clear();
  xpositions.clear();
  ypositions.clear();
  widths.clear();
  heights.clear();
}

bool TextRenderer::IsDescender(char32_t c) {
  if (c == U'g' || c == U'j' || c == U'p' || c == U'q' || c == U'y') {
    return true;
  } else {
    float bearingY =
        TextRenderer::characterMap.at(c).at(CharStyle::REGULAR).Bearing.y;
    float sizeY =
        TextRenderer::characterMap.at(c).at(CharStyle::REGULAR).Size.y;
    return bearingY - sizeY < 0.f;
  }
}
