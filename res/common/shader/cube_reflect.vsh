#version 330 core
in vec3 a_position;
in vec3 a_normal;

uniform mat4 u_matWorld;
uniform mat4 u_matWorldViewProj;

out vec3 v_position;
out vec3 v_normal;

void main()
{
	gl_Position = u_matWorldViewProj * vec4(a_position, 1.0);
	v_normal = normalize(mat3(u_matWorld) * a_normal);
	v_position = (u_matWorld * vec4(a_position, 1.0)).xyz;
}
