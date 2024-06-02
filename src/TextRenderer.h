#pragma once
#include <glad/glad.h>
#include <ft2build.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include FT_FREETYPE_H

#include "ConfigManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
  unsigned int TextureID;  // ID handle of the glyph texture
  glm::ivec2 Size;         // size of glyph
  glm::ivec2 Bearing;      // offset from baseline to left/top of glyph
  unsigned int Advance;    // horizontal offset to advance to next glyph
};

// A renderer class for rendering text displayed by a font loaded using the
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer : public Renderer {
 public:
  // holds a list of pre-compiled Characters in regular, bold, and italic.
  static std::unordered_map<char32_t, std::unordered_map<CharStyle, Character>>
      characterMap;
  // counts the number of times the character has been loaded
  static std::unordered_map<char32_t, int> characterCount;
  //  shader used for text rendering
  Shader TextShader;
  // constructor
  TextRenderer(const Shader& shader, unsigned int width, unsigned int height,
               char32_t benchmarkChar = U'H');
  // pre-compiles a list of characters from the given font
  static void Load(std::string font, unsigned int fontSize,
                   CharStyle charStyle = CharStyle::REGULAR,
                   const std::vector<FT_ULong> charactersToLoad = {});
  // pre-compiles a list of characters
  static void Load(const std::u32string& charactersToLoad);
  static void UnLoadIfNotUsed(char32_t character);
  static void UnLoadIfNotUsed(const std::u32string& charactersToLoad);

  // Get the height and width of the text
  virtual std::pair<glm::vec3, bool> GetTextSize(std::u32string text,
                                                 float scale, float lineWidth,
                                                 float lineSpacingFactor,
                                                 float additionalPadding) = 0;
  // renders a string of text using the precompiled list of characters, return
  // the bottom left corner of the rendered text and the spacing between lines
  virtual std::pair<float, float> RenderText(std::u32string text, float x,
                                             float y, float scale,
                                             float lineWidth,
                                             float lineSpacingFactor,
                                             float additionalPadding,
                                             glm::vec3 color, float alpha) = 0;
  virtual std::pair<float, float> RenderCenteredText(
      std::u32string text, float x, float y, float scale, float lineWidth,
      float lineSpacingFactor, float additionalPadding, glm::vec2 center,
      glm::vec3 color, float alpha) = 0;

  char32_t GetBenchmarkChar() const { return benchmarkChar; }

 protected:
  // spaces per tab
  const size_t tabSize = 4;
  // the specific character as a reference or standard for setting the vertical
  // alignment and spacing of text
  char32_t benchmarkChar = U'H';
  // replaces all tabs with spaces
  std::u32string replaceTabs(const std::u32string& text);
  // Removes '{', '}', '[', ']' from the strign and return an array indicating
  // which characters are bold, italic or regular.
  std::pair<std::u32string, std::vector<CharStyle>> getEachCharacterStyle(
      const std::u32string& text, CharStyle initialStyle = CharStyle::REGULAR);
  // Renders a single character, assuming the character is pre-compiled
  void RenderChar(Character ch, float x, float y, float w, float h);
  // Renders a line of characters, assuming the characters are pre-compiled
  void RenderLine(std::vector<Character>& line, std::vector<float>& xpositions,
                  std::vector<float>& ypositions, std::vector<float>& widths,
                  std::vector<float>& heights,
                  glm::vec2 offset = glm::vec2(0.f));
  // Checks if the character is a descender.
  bool IsDescender(char32_t c);
};