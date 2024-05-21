#include "TextRenderer.h"

std::unordered_map<CharStyle, std::map<char32_t, Character>> TextRenderer::characterMap;

void TextRenderer::Load(std::string font, unsigned int fontSize,CharStyle charStyle, const std::pair<FT_ULong, FT_ULong>& range) {
  //// first clear the previously loaded Characters
  // if (characterMap.count(charStyle)) {
  //   characterMap[charStyle].clear();
  // }
  //  then initialize and load the FreeType library
  FT_Library ft;
  if (FT_Init_FreeType(&ft))  // all functions return a value different than 0
                              // whenever an error occurred
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
              << std::endl;
  // load font as face
  FT_Face face;
  if (FT_New_Face(ft, font.c_str(), 0, &face))
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
  // set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, fontSize);
  // disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  //std::vector<FT_ULong> charactersToLoad;
  //// Add the first 128 ASCII characters
  //for (FT_ULong c = 0; c < 128; ++c) {
  //  charactersToLoad.emplace_back(c);
  //}

  // pre - load / compile characters and store them
  for (auto c = range.first; c < range.second; ++c) {
    // load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
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
    characterMap[charStyle][c] = character;
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  // destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

void TextRenderer::Load(std::string font, unsigned int fontSize,
  CharStyle charStyle,
  const std::vector<std::pair<FT_ULong, FT_ULong>>& ranges) {
  for (const auto& range : ranges) {
	Load(font, fontSize, charStyle, range);
  }
}

void TextRenderer::LoadLanguage(std::string font, unsigned int fontSize, CharStyle charStyle, Language languae) {
  switch (languae) {
    case Language::ENGLISH:
      TextRenderer::Load(font, fontSize, charStyle, {0x0000, 0x007F});
      break;
    case Language::FRENCH:
      TextRenderer::Load(font, fontSize, charStyle, {0x0000, 0x007F});
      TextRenderer::Load(font, fontSize, charStyle, {0x0080, 0x00FF});
      break;
	case Language::CHINESE_SIMPLIFIED:
      TextRenderer::Load(font, fontSize, charStyle, {0x0000, 0x007F});
	  break;
    case Language::CHINESE_TRADITIONAL:
      TextRenderer::Load(font, fontSize, charStyle, {0x0000, 0x007F});
	  break;
	case Language::KOREAN:
      TextRenderer::Load(font, fontSize, charStyle, {0x0000, 0x007F});
	  break;
    case Language::JAPANESE:
	  TextRenderer::Load(font, fontSize, charStyle, {0x0000, 0x007F});
    default:
      break;
  }
}

void TextRenderer::LoadPreferredLanguage(unsigned int fontSize, CharStyle charStyle) {
  ConfigManager& config = ConfigManager::GetInstance();
  this->LoadLanguage(config.GetFontFilePath(charStyle), fontSize, charStyle, config.GetLanguage());
}


TextRenderer::TextRenderer(const Shader& shader, unsigned int width,
                           unsigned int height)
    : Renderer(shader) {
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

std::string TextRenderer::replaceTabs(const std::string& text) {
  std::string result;
  for (const auto& c : text) {
    if (c == '\t') {
      result.append(tabSize, ' ');
    } else {
      result.append(1, c);
    }
  }
  return result;
}

std::pair<std::string, std::vector<CharStyle> >
TextRenderer::getEachCharacterStyle(const std::string& text,
                                    CharStyle initialStyle) {
  std::string tmp = replaceTabs(text);
  std::vector<CharStyle> resultStyle;
  std::string resultText;
  CharStyle currentStyle = initialStyle;
  for (const auto& c : tmp) {
    if (c == '{') {
      if (currentStyle == CharStyle::ITALIC) {
        currentStyle = CharStyle::BOLD_ITALIC;
      } else {
        currentStyle = CharStyle::BOLD;
      }
    } else if (c == '[') {
      if (currentStyle == CharStyle::BOLD) {
        currentStyle = CharStyle::BOLD_ITALIC;
      } else {
        currentStyle = CharStyle::ITALIC;
      }
    } else if (c == '}') {
      if (currentStyle == CharStyle::BOLD_ITALIC) {
        currentStyle = CharStyle::ITALIC;
      } else {
        currentStyle = CharStyle::REGULAR;
      }
    } else if (c == ']') {
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
