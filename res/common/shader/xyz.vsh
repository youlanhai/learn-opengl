#version 330 core
in vec4 a_position;

uniform mat4 u_matWorldViewProj;

void main()
{
	gl_Position = u_matWorldViewProj * a_position;
}
