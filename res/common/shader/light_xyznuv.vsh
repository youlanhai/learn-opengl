attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord0;

uniform mat4 MVP;

uniform vec3 ambientColor;

uniform vec3 lightDir;
uniform vec3 lightColor;

varying vec2 v_texcoord;
varying vec4 v_color;

vec3 light()
{
	float diffuse = lightDir * a_normal;
	return ambientColor + lightColor * max(0, diffuse);
}

void main()
{
	gl_Position = MVP * a_position;
	v_texcoord = a_texcoord0;
	v_color = vec4(light(), 1.0);
}
