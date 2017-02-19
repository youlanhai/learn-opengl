
uniform sampler2D u_texture0;

varying vec2 v_texcoord;
varying vec4 v_color;

void main()
{
	gl_FragColor = texture2D(u_texture0, v_texcoord) * v_color;
}
