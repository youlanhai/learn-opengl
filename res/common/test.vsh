attribute vec4 a_position;
attribute vec4 a_color;

uniform mat4 matWorldViewProj;

varying vec4 v_color;

void main()
{
	gl_Position = matWorldViewProj * a_position;
	v_color = a_color;
}
