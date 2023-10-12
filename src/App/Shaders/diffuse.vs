#version 330 core

layout(location=0) in vec2 pos;
out vec2 vert_pos;

uniform vec2 shift;
uniform float scale;

void main() {
	vert_pos = vec2(pos.x * scale + shift.x, pos.y * scale + shift.y);
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}
