attribute vec4 a_position;

uniform mat4 u_matWorldViewProj;

varying vec3 texCoord;

void main()
{
	gl_Position = u_matWorldViewProj * a_position;
	texCoord = a_position.xyz;
}
