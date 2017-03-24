#version 330 core
out vec4 FragColor;
uniform samplerCube u_texture0;

in vec3 texCoord;

void main()
{
	FragColor = texture(u_texture0, texCoord);
}
