#version 330 core
in vec4 a_position;
in vec3 a_normal;
in vec2 a_texcoord0;
in vec3 a_tangent;

uniform mat4 u_matWorldViewProj;
uniform mat4 u_matWorld;

out vec2 v_texcoord;
out mat3 v_TBN;

void main()
{
	gl_Position = u_matWorldViewProj * a_position;
	v_texcoord = a_texcoord0;

	vec3 T = normalize(vec3(u_matWorld * vec4(a_tangent, 0.0)));
	vec3 N = normalize(vec3(u_matWorld * vec4(a_normal, 0.0)));
	vec3 B = normalize(cross(T, N));

	v_TBN = mat3(T, B, N);
}
