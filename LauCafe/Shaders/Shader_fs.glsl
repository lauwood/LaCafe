#version 400

in vec4 frag_Color;
uniform float red = 0.0;
uniform float blue = 0.0;
layout(location=0) out vec4 out_Color;

void main(void)
{
	if(blue != 0.0) {
		out_Color = vec4 (frag_Color.x, frag_Color.y, blue, 1.0);
	} else {
		out_Color = frag_Color;
	}

	if(red != 0.0) {
		out_Color = vec4 (red, frag_Color.y, frag_Color.z, 1.0);
	}
}