#include "Text.h"

// Code has been refrenced from the LearnOpenGL online tutorials 

void Text::intShader(glm::mat4 projection)
{
    textShader = Shader::GetInstance()->CreateProgram("shaders/text.vert", "shaders/text.frag");

    glUseProgram(textShader);
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUseProgram(0);

}

void Text::InitTextRendering()
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("FREETYPE initialization failed");
        return;
    }

    std::string fontPath = "fonts/arial.ttf";
    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        printf("Error loading font");
        FT_Done_FreeType(ft);
        return;
    }

    // Set font size (48pt)
    FT_Set_Pixel_Sizes(face, 0, 48);

    LoadCharacters(face);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    ConfigureTextRendering();
}

// Load characters into the map for text rendering
void Text::LoadCharacters(FT_Face face)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of the ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("Error loading Glyph");
            return;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

// Configure VAO/VBO for rendering texture quads
void Text::ConfigureTextRendering()
{
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Render text on the screen
void Text::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
    glUseProgram(textShader);
    glUniform3f(glGetUniformLocation(textShader, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // Iterate through all characters
    for (auto c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
