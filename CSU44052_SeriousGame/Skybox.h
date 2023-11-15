#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION

class Skybox
{
public:
	Skybox();

	Skybox(std::vector<std::string> faceLocations);

	void DrawSkybox(glm::mat4 cView, glm::mat4 projection_matrix);

	~Skybox();

private:
	Shader* skyShader;

	GLuint cubemapTexture;
	GLuint uniformProjection, uniformView;
	GLuint skyboxVAO, skyboxVBO, skyboxIBO;
	unsigned int skyboxProgram;
};

