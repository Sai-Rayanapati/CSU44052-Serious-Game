#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include "Model.h"
#include "Shader.h"
#include "glm/gtc/type_ptr.hpp"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character 
{
	unsigned int TextureID;
	glm::ivec2   Size; 
	glm::ivec2   Bearing; 
	unsigned int Advance; 
};

class Text
{
public:
	void intShader(glm::mat4 projection);

	void InitTextRendering();

	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);

private:
	void LoadCharacters(FT_Face face);

	void ConfigureTextRendering();

	std::map<GLchar, Character> Characters;
	unsigned int textVAO, textVBO;

	unsigned int textShader;



	
};

