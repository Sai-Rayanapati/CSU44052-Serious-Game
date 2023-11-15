#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

smooth in vec4 ioEyeSpacePosition;

void main()
{    
    FragColor = texture(skybox, texCoords);

    // fog rendering
    float fogCoordinate = length(ioEyeSpacePosition.xyz);
    float fogDensity = 1.0;
    vec3 fogColor = vec3(0.7);
    float fogFactor = exp(-pow(fogDensity * fogCoordinate, 2.0));
    fogFactor = 1.0 - clamp(fogFactor, 0.0, 1.0);

    FragColor = mix(FragColor, vec4(fogColor, 1.0), fogFactor);
}