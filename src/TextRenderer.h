#pragma once
#include <ft2build.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include FT_FREETYPE_H

#include "Renderer.h"
#include "ResourceManager.h"
#include "ConfigManager.h"
#include "Shader.h"
#include "Texture.h"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
  unsigned int TextureID;  // ID handle of the glyph texture
  glm::ivec2 Size;         // size of glyph
  glm::ivec2 Bearing;      // offset from baseline to left/top of glyph
  unsigned int Advance;    // horizontal offset to advance to next glyph
};

// enum of char styles
//enum CharStyle { Regular, Bold, Italic, BoldItalic };

// A renderer class for rendering text displayed by a font loaded using the
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer : public Renderer {
 public:
  // holds a list of pre-compiled Characters in regular, bold, and italic.
  static std::unordered_map<CharStyle, std::map<char32_t, Character> > characterMap;
  // std::map<char, Character> Characters;
  //// holds a list of pre-compiled Charecters in bold.
  // std::map<char, Character> BoldCharacters;
  //// holds a list of pre-compiled Charecters in italic.
  // std::map<char, Character> ItalicCharacters;
  //  shader used for text rendering
  Shader TextShader;
  // constructor
  TextRenderer(const Shader& shader, unsigned int width, unsigned int height);
  // pre-compiles a list of characters from the given font
  void Load(std::string font, unsigned int fontSize, CharStyle charStyle = CharStyle::REGULAR, const std::pair<FT_ULong, FT_ULong>& range = {0, 128});
  // pre-compiles multiple lists of characters from the given font
  void Load(std::string font, unsigned int fontSize,
            CharStyle charStyle = CharStyle::REGULAR,
            const std::vector<std::pair<FT_ULong, FT_ULong>>& range = {{0, 128}});
  void LoadLanguage(std::string font, unsigned int fontSize, CharStyle = CharStyle::REGULAR, Language languae = Language::ENGLISH);
  void LoadPreferredLanguage(unsigned int fontSize, CharStyle = CharStyle::REGULAR);

  // Get the height and width of the text
  virtual std::pair<glm::vec3, bool> GetTextSize(std::string text, float scale,
                                         float lineWidth,
                                         float lineSpacingFactor,
                                         float additionalPadding) = 0;
  // renders a string of text using the precompiled list of characters, return
  // the bottom left corner of the rendered text and the spacing between lines
  virtual std::pair<float, float> RenderText(std::string text, float x, float y,
                                     float scale, float lineWidth,
                                     float lineSpacingFactor,
                                     float additionalPadding, glm::vec3 color,
                                     float alpha) = 0;
  virtual std::pair<float, float> RenderCenteredText(
      std::string text, float x, float y, float scale, float lineWidth,
      float lineSpacingFactor, float additionalPadding,
      glm::vec2 center, glm::vec3 color, float alpha) = 0;

protected:
  // spaces per tab
  const size_t tabSize = 4;
  // replaces all tabs with spaces
  std::string replaceTabs(const std::string& text);
  // Remove '{', '}', '[', ']' from the strign and return an array indicating
  // which characters are bold, italic or regular.
  std::pair<std::string, std::vector<CharStyle> > getEachCharacterStyle(
      const std::string& text, CharStyle initialStyle = CharStyle::REGULAR);
  // Render a single character, assuming the character is pre-compiled
  void RenderChar(Character ch, float x, float y, float w, float h);
  // Render a line of characters, assuming the characters are pre-compiled
  void RenderLine(std::vector<Character>& line, std::vector<float>& xpositions,
                  std::vector<float>& ypositions, std::vector<float>& widths,
                  std::vector<float>& heights,
                  glm::vec2 offset = glm::vec2(0.f));
};