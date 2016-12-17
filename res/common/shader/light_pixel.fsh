varying vec2 v_texcoord;
varying vec3 v_normal;

uniform sampler2D u_texture0;

uniform vec3 ambientColor;

uniform vec3 lightDir;
uniform vec3 lightColor;

vec3 light()
{
	float diffuse = dot(lightDir, normalize(v_normal));
	return ambientColor + lightColor * max(0.0, diffuse);
}

void main()
{
	vec4 color = vec4(light(), 1.0);
	gl_FragColor = texture2D(u_texture0, v_texcoord) * color;
}
