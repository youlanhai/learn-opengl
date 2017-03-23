#version 330 core
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texcoord0;

uniform mat4 u_matWorld;
uniform mat4 u_matWorldView;
uniform mat4 u_matWorldViewProj;
uniform mat4 lightMatrix;

out vec2 v_texcoord;
out vec3 v_posInView;
out vec3 v_posInLight;
out vec3 v_normal;

void main()
{
	vec4 pos = vec4(a_position, 1.0);
	gl_Position = u_matWorldViewProj * pos;
	v_texcoord = a_texcoord0;

	v_normal = (u_matWorldView * vec4(a_normal, 0.0)).xyz;
	v_posInView = vec3(u_matWorldView * pos);

	v_posInLight = (lightMatrix * u_matWorld * pos).xyz;
}
