#version 330 core

in vec2 vert_pos;
out vec4 out_col;

uniform int iter;
uniform float bailOut;
uniform int color;

void main() {
	float x = vert_pos.x;
	float y = vert_pos.y;
	float squareBailOut = bailOut * bailOut;

	float zX = 0;
	float zY = 0;
	int i = 1;

	for (;i <= iter; i++) {
		float zX_ = zX * zX - zY * zY + x;
		float zY_ = 2 * zX * zY + y;
		zX = zX_;
		zY = zY_;

		if (zX * zX + zY * zY > squareBailOut) {	
			break;
		}
	}

	if (i <= iter) {
		float r = (color * i) % 255;
		float g = ((color + 1) * i) % 255;
		float b = ((color + 2) * i) % 255;
		out_col = vec4(r / 255, g / 255, b / 255, 1.0);
	} else {
		out_col = vec4(1.0, 1.0, 1.0, 1.0);
	}
}