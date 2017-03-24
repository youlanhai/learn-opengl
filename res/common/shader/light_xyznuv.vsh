#version 330 core
in vec4 a_position;
in vec3 a_normal;
in vec2 a_texcoord0;

uniform mat4 u_matWorld;
uniform mat4 u_matWorldViewProj;

uniform vec3 u_ambientColor;

uniform vec3 lightDir;
uniform vec3 lightColor;

out vec2 v_texcoord;
out vec4 v_color;

vec3 light(vec3 normal)
{
	float diffuse = dot(lightDir, normalize(normal));
	return u_ambientColor + lightColor * max(0, diffuse);
}

void main()
{
	gl_Position = u_matWorldViewProj * a_position;
	v_texcoord = a_texcoord0;

	vec3 normal = (u_matWorld * vec4(a_normal, 0.0)).xyz;
	v_color = vec4(light(normal), 1.0);
}
