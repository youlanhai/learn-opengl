attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord0;

uniform mat4 MVP;

varying vec2 v_texcoord;
varying vec4 v_color;

void main()
{
	gl_Position = MVP * a_position;
	v_texcoord = a_texcoord0;
	v_normal = a_normal;
}
