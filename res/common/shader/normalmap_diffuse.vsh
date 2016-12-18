attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord0;
attribute vec3 a_tangent;

uniform mat4 matWorld;
uniform mat4 matMVP;
uniform vec3 lightDir;

varying vec3 v_lightDir;
varying vec2 v_texcoord;

void main()
{
	gl_Position = matMVP * a_position;
	v_texcoord = a_texcoord0;

	vec3 T = normalize(vec3(matWorld * vec4(a_tangent, 0.0)));
	vec3 N = normalize(vec3(matWorld * vec4(a_normal, 0.0)));
	vec3 B = normalize(cross(T, N));

	//计算逆矩阵。
	mat3 TBN = transpose(mat3(T, B, N));

	//将灯光方向转换到切线空间
	v_lightDir = TBN * lightDir;
}
