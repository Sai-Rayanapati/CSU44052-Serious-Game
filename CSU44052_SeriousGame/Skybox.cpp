#include "Skybox.h"


Skybox::Skybox()
{
}

Skybox::Skybox(std::vector<std::string> faceLocations)
{
	// Set up the shaders for skybox 
	skyboxProgram = Shader::GetInstance()->CreateProgram("shaders/skybox.vert", "shaders/skybox.frag");

	// Mesh Setup
	unsigned int skyboxIndices[] = 
	{
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	GLfloat skyboxVertices[] = 
	{
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
	};

	//  initialize mesh for the skybox
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxIBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Texture setup
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	stbi_set_flip_vertically_on_load(false);

	int width, height, bitDepth;

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, 0);
		if (texData)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
			stbi_image_free(texData);
		}
		else
		{
			printf("Failed to find: %s\n", faceLocations[i].c_str());
			stbi_image_free(texData);
			return;
		}
	}
}


void Skybox::DrawSkybox(glm::mat4 view_matrix, glm::mat4 projection_matrix)
{
	glUseProgram(skyboxProgram);

	// Set view and projection matrices
	unsigned int viewLoc = glGetUniformLocation(skyboxProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	unsigned int projectionLoc = glGetUniformLocation(skyboxProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	// Draw the skybox
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

Skybox::~Skybox()
{
}