#version 330 core
out vec4 FragColor;
uniform sampler2D u_texture0;

in vec2 v_texcoord;
in vec4 v_color;

void main()
{
	FragColor = texture(u_texture0, v_texcoord) * v_color;
}
