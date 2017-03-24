#version 330 core
out vec4 FragColor;

uniform samplerCube u_texture0;
uniform vec3 u_cameraPos;

in vec3 v_position;
in vec3 v_normal;

void main()
{
	vec3 viewDir = normalize(v_position - u_cameraPos);
	vec3 refectDir = reflect(viewDir, normalize(v_normal));
	
	FragColor = texture(u_texture0, refectDir);
}
