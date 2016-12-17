attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord0;

uniform mat4 matWorld;
uniform mat4 matMVP;

uniform vec3 ambientColor;

uniform vec3 lightDir;
uniform vec3 lightColor;

varying vec2 v_texcoord;
varying vec4 v_color;

vec3 light(vec3 normal)
{
	float diffuse = dot(lightDir, normalize(normal));
	return ambientColor + lightColor * max(0, diffuse);
}

void main()
{
	gl_Position = matMVP * a_position;
	v_texcoord = a_texcoord0;

	vec3 normal = (matWorld * vec4(a_normal, 0.0)).xyz;
	v_color = vec4(light(normal), 1.0);
}
