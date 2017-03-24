#version 330 core
in vec4 a_position;
in vec3 a_normal;
in vec2 a_texcoord0;
in vec3 a_tangent;

uniform mat4 u_matWorld;
uniform mat4 u_matWorldViewProj;
uniform vec3 lightDir;

out vec3 v_lightDir;
out vec2 v_texcoord;

void main()
{
	gl_Position = u_matWorldViewProj * a_position;
	v_texcoord = a_texcoord0;

	vec3 T = normalize(vec3(u_matWorld * vec4(a_tangent, 0.0)));
	vec3 N = normalize(vec3(u_matWorld * vec4(a_normal, 0.0)));
	vec3 B = normalize(cross(T, N));

	//计算逆矩阵。
	mat3 TBN = transpose(mat3(T, B, N));

	//将灯光方向转换到切线空间
	v_lightDir = TBN * lightDir;
}
