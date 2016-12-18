attribute vec4 a_position;
attribute vec2 a_texcoord0;

uniform mat4 u_matWorldViewProj;

varying vec2 v_texcoord;

void main()
{
	gl_Position = u_matWorldViewProj * a_position;
	v_texcoord = vec2(a_texcoord0.x, 1.0 - a_texcoord0.y);
}
