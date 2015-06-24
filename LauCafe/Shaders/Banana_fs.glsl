#version 400

in vec4 frag_Color;
layout(location=0) out vec4 out_Color;

void main(void)
{
	out_Color = vec4(frag_Color.x, frag_Color.y, 0.0, 1.0);
}