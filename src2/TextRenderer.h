#pragma once
#include <map>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"
#include "ResourceManager.h"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // size of glyph
    glm::ivec2   Bearing;   // offset from baseline to left/top of glyph
    unsigned int Advance;   // horizontal offset to advance to next glyph
};

// enum of char styles
enum CharStyle {
	Regular,
	Bold,
	Italic,
    BoldItalic
};

// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer: public Renderer
{
public:
    // holds a list of pre-compiled Characters in regular, bold, and italic.
    std::unordered_map<CharStyle, std::map<char, Character> > characterMap;
    //std::map<char, Character> Characters;
    //// holds a list of pre-compiled Charecters in bold.
    //std::map<char, Character> BoldCharacters;
    //// holds a list of pre-compiled Charecters in italic.
    //std::map<char, Character> ItalicCharacters;
    // shader used for text rendering
    Shader TextShader;
    // constructor
    TextRenderer(const Shader& shader, unsigned int width, unsigned int height);
    // pre-compiles a list of characters from the given font
    void Load(std::string font, unsigned int fontSize, CharStyle = CharStyle::Regular);
    // Get the height and width of the text
    glm::vec3 GetTextSize(std::string text, float scale, float lineWidth, float lineSpacingFactor = 1.2f, float additionalPadding = 10.0f);
    // renders a string of text using the precompiled list of characters, return the bottom left corner of the rendered text and the spacing between lines
    std::pair<float, float> RenderText(std::string text, float x, float y, float scale, float lineWidth, float lineSpacingFactor = 1.2f, float additionalPadding = 10.0f, glm::vec3 color = glm::vec3(0.0f), float alpha = 1.0f);
    std::pair<float, float> RenderCenteredText(std::string text, float x, float y, float scale, float lineWidth, float lineSpacingFactor = 1.2f, float additionalPadding = 10.0f, glm::vec2 center = kWindowSize*0.5f, glm::vec3 color = glm::vec3(0.0f), float alpha = 1.0f);
	
private:
    // spaces per tab
    const size_t tabSize = 4;
    // replaces all tabs with spaces
    std::string replaceTabs(const std::string& text);
    // Remove '{', '}', '[', ']' from the strign and return an array indicating which characters are bold, italic or regular.
    std::pair<std::string, std::vector<CharStyle> > getEachCharacterStyle(const std::string& text, CharStyle initialStyle = CharStyle::Regular);
    // Render a single character, assuming the character is pre-compiled
    void RenderChar(Character ch, float x, float y, float w, float h);
    // Render a line of characters, assuming the characters are pre-compiled
    void RenderLine(std::vector<Character>& line, std::vector<float>& xpositions, std::vector<float>& ypositions, std::vector<float>& widths, std::vector<float>& heights, glm::vec2 offset = glm::vec2(0.f));
};