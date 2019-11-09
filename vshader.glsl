#version 330

in vec4 vPosition;
in vec4 vColor;

out vec4 outColor;

void main()
{
	gl_Position = vPosition;
	outColor = vColor;
}
