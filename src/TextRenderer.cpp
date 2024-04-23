#include "TextRenderer.h"


TextRenderer::TextRenderer(const Shader& shader, unsigned int width, unsigned int height): Renderer(shader)
{
    // load and configure shader
    this->shader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f), true);
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

void TextRenderer::Load(std::string font, unsigned int fontSize, CharStyle charStyle)
{
    // first clear the previously loaded Characters
    if (characterMap.count(charStyle)) {
    	characterMap[charStyle].clear();
    }
    // then initialize and load the FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) // all functions return a value different than 0 whenever an error occurred
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::vector<FT_ULong> charactersToLoad;
    // Add the first 128 ASCII characters
    for (FT_ULong c = 0; c < 128; ++c) {
        charactersToLoad.emplace_back(c);
    }

    // pre - load / compile characters and store them
    for (const auto& c : charactersToLoad)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
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
            face->glyph->advance.x
        };
        characterMap[charStyle][c] = character;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

glm::vec3 TextRenderer::GetTextSize(std::string text, float scale, float lineWidth, float lineSpacingFactor, float additionalPadding) {
    glm::vec2 finalSize = glm::vec2(0.f);

    // iterate through all characters
    std::string::const_iterator c;
    // Find the word's start (inclusive) and end (exlusive) index.
    std::pair<int, int> wordIndex = findWord(text, 0);

    // Memorize the initial x and y position.
    float x = 0.f, y = 0.f;
    float initialX = x;

    float lenOfLine = 0.f;

    // Space between lines
    float lineSpacing = characterMap[CharStyle::Regular]['H'].Size.y * scale * lineSpacingFactor + additionalPadding;

    //  A boolean value for whether the word is the first word of the line.
    bool firstWord = true;

    // char style
    CharStyle charStyle = CharStyle::Regular;

    std::string lastWord;

    while (wordIndex.second > wordIndex.first) {

        std::string word = text.substr(wordIndex.first, wordIndex.second - wordIndex.first);
        auto wordStyles = getEachCharacterStyle(word, charStyle);
        word = wordStyles.first;
        auto styles = wordStyles.second;
        charStyle = styles.back();

        // Get the x where the word ends.
        float endX = x;
        for (c = word.begin(); c != word.end(); ++c)
        {
            charStyle = styles[c - word.begin()];
            Character ch = characterMap[charStyle][*c];
            // If it is the last character of the word, then add the bearing.x and size.x to the endX.
            if (c == std::prev(word.end())) {
                endX += (ch.Bearing.x + ch.Size.x) * scale;
            }
            else {
                // now advance cursors for next glyph
                endX += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
            }
        }

        lastWord = word;
        if (lenOfLine + endX - x > lineWidth) {
            y += lineSpacing;
            x = initialX;
            finalSize.x = std::max(finalSize.x, lenOfLine);
            lenOfLine = 0.f;
            if (firstWord) {
                std::cout << "word: " << lastWord << " lenOfLine: " << lenOfLine << " endX: " << endX << " x: " << x << " initialX: " << initialX << " lineWidth: " << lineWidth << std::endl;
            }
            assert(!firstWord && "The width of the line should at least have space for one word.");
            firstWord = true;
        }

        // Move the end index of the word to the start of the next word.
        while (wordIndex.second < text.size() && text[wordIndex.second] == ' ') {
            ++wordIndex.second;
            word.append(" ");
            styles.emplace_back(styles.back());
        }
        for (c = word.begin(); c != word.end(); ++c)
        {
            charStyle = styles[c - word.begin()];
            Character ch = characterMap[charStyle][*c];
            // now advance cursors for next glyph
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
            lenOfLine = x - initialX;
        }

        // Get the next word.
        wordIndex = findWord(text, wordIndex.second);
        // If it is the end of the line, move to the next line.
        while (wordIndex.second == wordIndex.first && wordIndex.second < text.length() && text[wordIndex.first] == '\n') {
            y += lineSpacing;
            x = initialX;
            finalSize.x = std::max(finalSize.x, lenOfLine);
            lenOfLine = 0.f;
            firstWord = true;
            ++wordIndex.second;
            while (wordIndex.second < text.length() && text[wordIndex.second] == ' ') {
                ++wordIndex.second;
            }
            wordIndex = findWord(text, wordIndex.second);
        }
        firstWord = false;
    }
    finalSize.y = y + characterMap[CharStyle::Regular]['H'].Size.y * scale;
    return glm::vec3(finalSize, lineSpacing);
}

std::pair<float, float> TextRenderer::RenderText(std::string text, float x, float y, float scale, float lineWidth, float lineSpacingFactor, float additionalPadding, glm::vec3 color, float alpha)
{
    // activate corresponding render state	
    this->shader.Use();
    this->shader.SetVector3f("textColor", color);
    this->shader.SetFloat("alpha", alpha);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    // iterate through all characters
    std::string::const_iterator c;
    // Find the word's start (inclusive) and end (exlusive) index.
    std::pair<int, int> wordIndex = findWord(text, 0);
    
    // Memorize the initial x and y position.
    float initialX = x;
    
    float lenOfLine = 0.f;

    // Space between lines
    float lineSpacing = characterMap[CharStyle::Regular]['H'].Size.y * scale * lineSpacingFactor + additionalPadding;

    //  A boolean value for whether the word is the first word of the line.
    bool firstWord = true;

    // char style
    CharStyle charStyle = CharStyle::Regular;

    std::string lastWord;

    while (wordIndex.second > wordIndex.first) {

        std::string word = text.substr(wordIndex.first, wordIndex.second - wordIndex.first);
        auto wordStyles = getEachCharacterStyle(word, charStyle);
        word = wordStyles.first;
        auto styles = wordStyles.second;
        charStyle = styles.back();

        // Get the x where the word ends.
        float endX = x;
        for (c = word.begin(); c != word.end(); ++c)
        {   
            charStyle = styles[c - word.begin()];
            Character ch = characterMap[charStyle][*c];
            // If it is the last character of the word, then add the bearing.x and size.x to the endX.
            if (c == std::prev(word.end())) {
				endX += (ch.Bearing.x + ch.Size.x) * scale;
			}
            else {
                // now advance cursors for next glyph
				endX += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
			}
        }

        lastWord = word;
        if (lenOfLine + endX - x > lineWidth) {
            y += lineSpacing;
            x = initialX;
            lenOfLine = 0.f;
            if (firstWord) {
                std::cout << "word: " << lastWord << " lenOfLine: " << lenOfLine << " endX: " << endX << " x: " << x << " initialX: " << initialX << " lineWidth: " << lineWidth << std::endl;
            }
            assert(!firstWord && "The width of the line should at least have space for one word.");
            firstWord = true;
		}

        // Move the end index of the word to the start of the next word.
        while (wordIndex.second < text.size() && text[wordIndex.second] == ' ') {
            ++wordIndex.second;
            word.append(" ");
            styles.emplace_back(styles.back());
        }
        for (c = word.begin(); c != word.end(); ++c)
        {
            charStyle = styles[c - word.begin()];
            Character ch = characterMap[charStyle][*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y + (this->characterMap[charStyle]['H'].Bearing.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 0.0f },
                { xpos,     ypos,       0.0f, 0.0f },

                { xpos,     ypos + h,   0.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 0.0f }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
            lenOfLine = x - initialX;
        }

        // Get the next word.
        wordIndex = findWord(text, wordIndex.second);
        // If it is the end of the line, move to the next line.
        while (wordIndex.second == wordIndex.first && wordIndex.second < text.length() && text[wordIndex.first] == '\n') {
        	y += lineSpacing;
			x = initialX;
			lenOfLine = 0.f;
			firstWord = true;
            ++wordIndex.second;
            while (wordIndex.second < text.length() && text[wordIndex.second] == ' ') {
            	++wordIndex.second;
            }
			wordIndex = findWord(text, wordIndex.second);
        }
        firstWord = false;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return std::make_pair(y + characterMap[CharStyle::Regular]['H'].Size.y * scale, lineSpacing);
}

std::pair<float, float> TextRenderer::RenderCenteredText(std::string text, float x, float y, float scale, float lineWidth, float lineSpacingFactor, float additionalPadding, glm::vec2 center, glm::vec3 color, float alpha)
{
    // activate corresponding render state	
    this->shader.Use();
    this->shader.SetVector3f("textColor", color);
    this->shader.SetFloat("alpha", alpha);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    // iterate through all characters
    std::string::const_iterator c;
    // Find the word's start (inclusive) and end (exlusive) index.
    std::pair<int, int> wordIndex = findWord(text, 0);

    // Memorize the initial x and y position.
    float initialX = x;

    float lenOfLine = 0.f;

    // Space between lines
    float lineSpacing = characterMap[CharStyle::Regular]['H'].Size.y * scale * lineSpacingFactor + additionalPadding;
    
    //  A boolean value for whether the word is the first word of the line.
    bool firstWord = true;

    // char style
    CharStyle charStyle = CharStyle::Regular;

    std::string lastWord;

    std::vector<Character> characters;
    std::vector<float> xpositions, ypositions, widths, heights;
    glm::vec2 offset = glm::vec2(0.f, center.y - (y + characterMap[CharStyle::Regular]['H'].Size.y * scale * 0.5f));

    while (wordIndex.second > wordIndex.first) {

        std::string word = text.substr(wordIndex.first, wordIndex.second - wordIndex.first);
        auto wordStyles = getEachCharacterStyle(word, charStyle);
        word = wordStyles.first;
        auto styles = wordStyles.second;
        charStyle = styles.back();

        // Get the x where the word ends.
        float endX = x;
        for (c = word.begin(); c != word.end(); ++c)
        {
            charStyle = styles[c - word.begin()];
            Character ch = characterMap[charStyle][*c];
            // If it is the last character of the word, then add the bearing.x and size.x to the endX.
            if (c == std::prev(word.end())) {
                endX += (ch.Bearing.x + ch.Size.x) * scale;
            }
            else {
                // now advance cursors for next glyph
                endX += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
            }
        }

        lastWord = word;
        if (lenOfLine + endX - x > lineWidth) {
            offset.x = center.x - (initialX + lenOfLine * 0.5f);
            RenderLine(characters, xpositions, ypositions, widths, heights, offset);
            y += lineSpacing;
            x = initialX;
            lenOfLine = 0.f;
            assert(!firstWord && "The width of the line should at least have space for one word.");
            firstWord = true;
        }

        // Move the end index of the word to the start of the next word.
        while (wordIndex.second < text.size() && text[wordIndex.second] == ' ') {
            ++wordIndex.second;
            word.append(" ");
            styles.emplace_back(styles.back());
        }

        for (c = word.begin(); c != word.end(); ++c)
        {
            charStyle = styles[c - word.begin()];
            Character ch = characterMap[charStyle][*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y + (this->characterMap[charStyle]['H'].Bearing.y - ch.Bearing.y) * scale;
            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // store the position, width, and height of the character.
            characters.emplace_back(ch);
            xpositions.emplace_back(xpos);
            ypositions.emplace_back(ypos);
            widths.emplace_back(w);
            heights.emplace_back(h);
            //// update VBO for each character
            //float vertices[6][4] = {
            //    { xpos,     ypos + h,   0.0f, 1.0f },
            //    { xpos + w, ypos,       1.0f, 0.0f },
            //    { xpos,     ypos,       0.0f, 0.0f },

            //    { xpos,     ypos + h,   0.0f, 1.0f },
            //    { xpos + w, ypos + h,   1.0f, 1.0f },
            //    { xpos + w, ypos,       1.0f, 0.0f }
            //};
            //// render glyph texture over quad
            //glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            //// update content of VBO memory
            //glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
            //glBindBuffer(GL_ARRAY_BUFFER, 0);
            //// render quad
            //glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
            lenOfLine = x - initialX;
        }

        // Get the next word.
        wordIndex = findWord(text, wordIndex.second);
        // If it is the end of the line, move to the next line.
        while (wordIndex.second == wordIndex.first && wordIndex.second < text.length() && text[wordIndex.first] == '\n') {
            offset.x = center.x - (initialX + lenOfLine * 0.5f);
            RenderLine(characters, xpositions, ypositions, widths, heights, offset);
            y += lineSpacing;
            x = initialX;
            lenOfLine = 0.f;
            firstWord = true;
            ++wordIndex.second;
            while (wordIndex.second < text.length() && text[wordIndex.second] == ' ') {
                ++wordIndex.second;
            }
            wordIndex = findWord(text, wordIndex.second);
        }
        firstWord = false;
    }

    offset.x = center.x - (initialX + lenOfLine * 0.5f);
    RenderLine(characters, xpositions, ypositions, widths, heights, offset);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return std::make_pair(y + characterMap[CharStyle::Regular]['H'].Size.y * scale, lineSpacing);
}

std::string TextRenderer::replaceTabs(const std::string& text)
{
	std::string result;
    for (const auto& c : text) {
        if (c == '\t') {
            result.append(tabSize, ' ');
		}
        else {
			result.append(1, c);
		}
	}
	return result;
}

std::pair<std::string, std::vector<CharStyle> > TextRenderer::getEachCharacterStyle(const std::string& text, CharStyle initialStyle)
{
    std::string tmp = replaceTabs(text);
	std::vector<CharStyle> resultStyle;
    std::string resultText;
    CharStyle currentStyle = initialStyle;
    for (const auto& c : tmp) {
        if (c == '{') {
            if (currentStyle == CharStyle::Italic) {
				currentStyle = CharStyle::BoldItalic;
            }
            else {
                currentStyle = CharStyle::Bold;
            }
		}
        else if (c == '[') {
            if (currentStyle == CharStyle::Bold) {
                currentStyle = CharStyle::BoldItalic;
            }
            else {
                currentStyle = CharStyle::Italic;
            }
		}
        else if (c == '}') {
            if (currentStyle == CharStyle::BoldItalic) {
				currentStyle = CharStyle::Italic;
            }
            else {
            	currentStyle = CharStyle::Regular;
            }
		}
        else if (c == ']') {
            if (currentStyle == CharStyle::BoldItalic) {
                currentStyle = CharStyle::Bold;
            }
            else {
                currentStyle = CharStyle::Regular;
            }
        }
        else {
			resultStyle.emplace_back(currentStyle);
            resultText.append(1, c);
		}
	}

    // push a char style at the end of the resultStyle to indicate the initial style of the next word.
    resultStyle.emplace_back(currentStyle);

    return std::make_pair(resultText, resultStyle);
}

void TextRenderer::RenderChar(Character ch, float x, float y, float w, float h)
{

    // update VBO for each character
    float vertices[6][4] = {
        { x,     y + h,   0.0f, 1.0f },
        { x + w, y,       1.0f, 0.0f },
        { x,     y,       0.0f, 0.0f },

        { x,     y + h,   0.0f, 1.0f },
        { x + w, y + h,   1.0f, 1.0f },
        { x + w, y,       1.0f, 0.0f }
    };
    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void TextRenderer::RenderLine(std::vector<Character>& line, std::vector<float>& xpositions, std::vector<float>& ypositions, std::vector<float>& widths, std::vector<float>& heights, glm::vec2 offset)
{
    for (size_t i = 0; i < line.size(); ++i) {
		RenderChar(line[i], xpositions[i] + offset.x, ypositions[i] + offset.y, widths[i], heights[i]);
	}
    line.clear();
	xpositions.clear();
	ypositions.clear();
	widths.clear();
    heights.clear();
}
