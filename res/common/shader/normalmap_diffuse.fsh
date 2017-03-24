#version 330 core
out vec4 FragColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform vec3 lightColor;
uniform vec3 u_ambientColor;

in vec2 v_texcoord;
in vec3 v_lightDir;

void main()
{
	vec3 normal = texture(u_texture1, v_texcoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec3 diffuse = lightColor * max(dot(normal, v_lightDir), 0.0);
	vec3 color = u_ambientColor + diffuse;

	FragColor = texture(u_texture0, v_texcoord) * vec4(color, 1.0);
}
