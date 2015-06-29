#version 400

in vec4 frag_Color;
layout(location=0) out vec4 out_Color;

void main(void)
{
	out_Color = frag_Color;
}