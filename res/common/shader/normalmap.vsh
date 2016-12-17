attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord0;
attribute vec3 a_tangent;

uniform mat4 matMVP;
uniform mat4 matWorld;

varying vec2 v_texcoord;
varying mat3 v_TBN;
varying vec3 v_tangent;

void main()
{
	gl_Position = matMVP * a_position;
	v_texcoord = a_texcoord0;

	vec3 T = normalize(vec3(matWorld * vec4(a_tangent, 0.0)));
	vec3 N = normalize(vec3(matWorld * vec4(a_normal, 0.0)));
	vec3 B = normalize(cross(T, N));

	v_TBN = mat3(T, B, N);
	v_tangent = T;
}
