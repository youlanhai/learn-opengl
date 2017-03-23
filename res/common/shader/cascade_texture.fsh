#version 330 core
out vec4 FragColor;
in vec2 v_texcoord;

uniform sampler2DArray u_texture0;
uniform int layerIndex;

void main()
{
	FragColor = texture(u_texture0, vec3(v_texcoord, layerIndex));
}
