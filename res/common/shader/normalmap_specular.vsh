attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord0;
attribute vec3 a_tangent;

uniform mat4 u_matWorld;
uniform mat4 u_matWorldViewProj;
uniform vec3 u_cameraPos;

uniform vec3 lightDir;

varying vec2 v_texcoord0;
varying vec3 v_viewDir;
varying vec3 v_lightDir;

void main()
{
	gl_Position = u_matWorldViewProj * a_position;
	v_texcoord0 = a_texcoord0;

	vec3 T = normalize(vec3(u_matWorld * vec4(a_tangent, 0.0)));
	vec3 N = normalize(vec3(u_matWorld * vec4(a_normal, 0.0)));
	vec3 B = normalize(cross(T, N));

	// 计算转置矩阵。对正交矩阵而言，转置矩阵就是拟矩阵。
	mat3 TBN = transpose(mat3(T, B, N));

	// 将世界空间中坐标和向量，都转换到切线空间中。
	vec3 tangentPos = TBN * vec3(u_matWorld * a_position);
	vec3 tangentCameraPos = TBN * u_cameraPos;
	v_viewDir = normalize(tangentCameraPos - tangentPos);

	v_lightDir = normalize(TBN * lightDir);
}
