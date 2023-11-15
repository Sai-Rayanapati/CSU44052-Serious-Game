#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

class Shader
{
public:
	Shader();

	~Shader();

	int CreateProgram(const std::string& vertexFilePath, const std::string& fragmentFilePath);

	static Shader* GetInstance();

private:
	bool ReadFile(const std::string& filePath, std::string& sourceCode);

	void ShowError(unsigned int shaderId, const std::string& shader);

	void Cleanup(unsigned int shaderId);

	static Shader* pShader;
};
