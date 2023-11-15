#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 norm;
layout(location = 3) in mat4 instanceModelMatrix; //Model matrix for each instance

// Output variables
out vec2 TexCoord;
out vec3 Normal;

// Uniform matrices
uniform mat4 model; 
uniform mat4 view;   
uniform mat4 projection;

// eye space pos for fog rendering
smooth out vec4 ioEyeSpacePosition;

void main() 
{
    //instance-specific model matrix
        mat4 newModel;
    if (any(notEqual(instanceModelMatrix[0], vec4(0.0))) ||
        any(notEqual(instanceModelMatrix[1], vec4(0.0))) ||
        any(notEqual(instanceModelMatrix[2], vec4(0.0))) ||
        any(notEqual(instanceModelMatrix[3], vec4(0.0))))
    {
        // Only apply the transformation if instanceModelMatrix is not an identity matrix
        newModel = model * instanceModelMatrix;
    }
    else
    {
        // If instanceModelMatrix is an identity matrix, use model directly
        newModel = model;
    }
    
    vec4 worldPos = newModel * vec4(pos, 1.0);
    gl_Position = projection * view * worldPos;
    
    TexCoord = tex;
    Normal = mat3(transpose(inverse(newModel))) * norm;

    mat4 mvMatrix = view * newModel;
    ioEyeSpacePosition = mvMatrix * vec4(pos, 1.0);
}
