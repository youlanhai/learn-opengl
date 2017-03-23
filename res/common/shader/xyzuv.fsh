#version 330 core
out vec4 FragColor;
in vec2 v_texcoord;

uniform sampler2D u_texture0;

void main()
{
	FragColor = texture(u_texture0, v_texcoord);
}
