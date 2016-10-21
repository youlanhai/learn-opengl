attribute vec4 position;
attribute vec4 color;

uniform mat4 matWorldViewProj;

varying vec4 v_color;

void main()
{
	gl_Position = matWorldViewProj * position;
	v_color = color;
}
