#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;


out vec2 TexCoord;
out vec3 Normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

smooth out vec4 ioEyeSpacePosition;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    TexCoord = texCoord;
    Normal = normal;

    mat4 mvMatrix = viewMatrix * modelMatrix;
    ioEyeSpacePosition = mvMatrix * vec4(position, 1.0);
}
