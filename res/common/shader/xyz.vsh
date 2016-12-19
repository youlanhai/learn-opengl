attribute vec4 a_position;

uniform mat4 matMVP;

void main()
{
	gl_Position = matMVP * a_position;
}
