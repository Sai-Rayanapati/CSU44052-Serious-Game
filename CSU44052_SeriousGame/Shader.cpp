#include "Shader.h"

Shader* Shader::pShader = nullptr;

Shader::Shader()
{
}

bool Shader::ReadFile(const std::string& filePath, std::string& sourceCode)
{
    std::ifstream file(filePath);
    if (!file)
    {
        printf("Failed to open file");
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    sourceCode = buffer.str();
    return true;
}

int Shader::CreateProgram(const std::string& vertexFilePath, const std::string& fragmentFilePath)
{
    std::string shader;

    if (!ReadFile(vertexFilePath, shader))
    {
        printf("Failed to read vertex shader file");
        return 1;
    }

    const char* vertexShaderSource = shader.c_str();
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    ShowError(vertexShader, "VERTEX SHADER");

    if (!ReadFile(fragmentFilePath, shader))
    {
        printf("Failed to read fragment shader file");
        return 1;
    }

    const char* fragmentShaderSource = shader.c_str();
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    ShowError(fragmentShader, "FRAGMENT SHADER");

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

Shader* Shader::GetInstance()
{
    if (pShader == nullptr)
    {
        pShader = new Shader();
    }
    return pShader;
}

void Shader::ShowError(unsigned int shaderId, const std::string& shader)
{
    int success;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
        printf("Error creating shader program \n");
        printf(infoLog);
    }
}

void Shader::Cleanup(unsigned int shaderId)
{
    glDeleteShader(shaderId);
}

Shader::~Shader()
{
}
