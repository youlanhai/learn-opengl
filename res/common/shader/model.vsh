attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord0;

uniform mat4 u_matWorldViewProj;
uniform mat4 u_matWorld;

varying vec2 v_texcoord;
varying vec3 v_normal;

void main()
{
	gl_Position = u_matWorldViewProj * a_position;
	v_texcoord = a_texcoord0;
	v_normal = (u_matWorld * vec4(a_normal, 0.0)).xyz;
}
