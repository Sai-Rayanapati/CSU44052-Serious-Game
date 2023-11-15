#version 330 core

in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D diffuseTexture;

const vec3 SUNLIGHT_DIRECTION = normalize(vec3(0.0f, -1.0f, -1.0f));
const vec3 SUNLIGHT_COLOUR = vec3(1.0f, 1.0f, 1.0f);

const vec3 AMBIENT_INTENSITY = vec3(0.1f);
const float DIFFUSED_INTENSITY = 0.8f;

uniform bool hasTexture;

uniform float shininess;
uniform float specularIntensity;

smooth in vec4 ioEyeSpacePosition;

void main() {
    // Sample diffuse and specular colors from textures
    vec4 textureColour = texture(diffuseTexture, TexCoord);

    vec4 specularColor = vec4(1.0, 1.0, 1.0, 1.0);

    // Calculate ambient color
    vec3 ambientColour = SUNLIGHT_COLOUR * textureColour.rgb * AMBIENT_INTENSITY;

    // Calculate diffuse color
    vec3 normal = normalize(Normal);
    float diffuseFactor = max(dot(normal, SUNLIGHT_DIRECTION), 0.0f);

    vec3 diffuseColorRGB = textureColour.rgb * DIFFUSED_INTENSITY * diffuseFactor * SUNLIGHT_COLOUR;

    // Calculate specular properties
    vec3 viewDir = normalize(-vec3(gl_FragCoord));
    vec3 reflectDir = reflect(-SUNLIGHT_DIRECTION, normal);
    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
    vec3 specularColorRGB = specularColor.rgb * specularIntensity * specularFactor;

    // Combine colors and output final color
    vec3 finalColorRGB = ambientColour + diffuseColorRGB + specularColorRGB;
    float alpha = textureColour.a;
    FragColor = vec4(finalColorRGB, alpha);

    // Fog Calculation
    float fogFactor = 0.0;
    float fogCoordinate = abs(ioEyeSpacePosition.z / ioEyeSpacePosition.w);
    float fogDensity = 0.1;
    vec3 fogColor = vec3(0.7);
    fogFactor = exp(-pow(fogDensity * fogCoordinate, 2.0));
    fogFactor = 1.0 - clamp(fogFactor, 0.0, 1.0);

    FragColor = mix(FragColor, vec4(fogColor, 1.0), fogFactor);
}
