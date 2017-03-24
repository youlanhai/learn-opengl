#version 330 core
out vec4 FragColor;
in vec2 v_texcoord;
in mat3 v_TBN;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;

uniform vec3 u_ambientColor;

uniform vec3 lightDir;
uniform vec3 lightColor;

void main()
{
	vec3 normal = texture(u_texture1, v_texcoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(v_TBN * normal);

	float diffuse = max(dot(lightDir, normal), 0.0);
	vec3 color = u_ambientColor + lightColor * diffuse;

	FragColor = texture(u_texture0, v_texcoord) * vec4(color, 1.0);
}
