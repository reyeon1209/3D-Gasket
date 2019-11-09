#version 330

in vec4 vPosition;
in vec4 vColor;

uniform int uTmp;
uniform mat4 uMat;

out vec4 fColor;

void main()
{
	gl_Position = uMat*vPosition;

	
	//if (uTmp == 0)	fColor = vec4(1.0, 0.0, 0.0, 1.0);
	//else if (uTmp == 1)	fColor = vec4(0.0. 1.0, 0.0, 1.0);

	fColor = vColor;
}
