#include "Texture.h"

Texture::Texture()
{
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";
}

Texture::Texture(char* fileLoc)
{
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = fileLoc;
}

void Texture::LoadTexture()
{
	glGenTextures(1, &textureID);

	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);
	if (texData)
	{
		GLenum format;

		if (bitDepth == 1) 
		{
			format = GL_RED;
		}
		else if (bitDepth == 3) 
		{
			format = GL_RGB;
		}
		else if (bitDepth == 4)
		{
			format = GL_RGBA;
		}
			
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(texData);
	}
	else
	{
		printf("Failed to find: %s\n", fileLocation);
		stbi_image_free(texData);
		return;
	}
}

void Texture::UseTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::ClearTexture()
{
	glDeleteTextures(1, &textureID);
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";
}


Texture::~Texture()
{
	ClearTexture();
}
