#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal_;
layout(location = 2) in vec2 textureCoord_;

uniform vec3 morphing;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 position;
out vec3 normal;
out vec2 textureCoord;

void main(){
	vec3 norm_vertex = normalize(vertex);
	vec4 mix1 = vec4(mix(vertex, norm_vertex, morphing.x), 1.0f);
	mat4 mvp = projection * view * model;
	vec3 mix2 = mix(normal_, norm_vertex, morphing.x);

	position = vec3(model * mix1);
	normal = normalize(mat3(transpose(inverse(model))) * mix2);
	textureCoord = textureCoord_;

	gl_Position = mvp * mix1;
}

