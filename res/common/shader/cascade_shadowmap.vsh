#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord0;

uniform mat4 u_matWorld;
uniform mat4 u_matWorldViewProj;
uniform mat4 lightMatrix;

out vec2 v_texcoord;
out vec3 v_posInWorld;
out vec4 v_posInLight;
out vec3 v_normal;

void main()
{
	vec4 pos = vec4(a_position, 1.0);
	gl_Position = u_matWorldViewProj * pos;
	v_texcoord = a_texcoord0;
	v_normal = (u_matWorld * vec4(a_normal, 0.0)).xyz;
	v_posInWorld = vec3(u_matWorld * pos);
	v_posInLight = lightMatrix * u_matWorld * pos;
}
