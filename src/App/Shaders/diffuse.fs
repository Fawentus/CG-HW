#version 330 core

in vec3 normal;
in vec3 position;
in vec2 textureCoord;

uniform sampler2D tex_2d;
uniform float ambientStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float specularStrength;

out vec4 color;

// https://habr.com/ru/articles/333932/
void main() {
	vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - position);

	float spec = pow(max(dot(normalize(viewPos - position), reflect(-lightDir, norm)), 0.0), 32);

    vec4 objectColor = texture(tex_2d, textureCoord);
    color = vec4(ambientStrength * lightColor + max(dot(norm, lightDir), 0.0) * lightColor + specularStrength * spec * lightColor, 1.0f) * objectColor;
}