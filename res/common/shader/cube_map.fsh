
uniform samplerCube u_texture0;

varying vec3 texCoord;

void main()
{
	gl_FragColor = texture(u_texture0, texCoord);
}
