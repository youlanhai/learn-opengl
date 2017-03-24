#version 330 core
out vec4 FragColor;
uniform sampler2D u_texture0; // 主纹理
uniform sampler2D u_texture1; // 法线贴图
uniform vec3 u_ambientColor;

uniform vec3 lightColor;
uniform float shininess;
uniform float specularStrength;

in vec2 v_texcoord0;
in vec3 v_viewDir;
in vec3 v_lightDir;

void main()
{
	vec3 normal = texture(u_texture1, v_texcoord0).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec3 diffuse = lightColor * max(dot(normal, v_lightDir), 0.0);
	//diffuse = (vec3)0;
#if 0
	//vec3 reflectDir = reflect(-v_lightDir, normal);
	//vec3 reflectDir = normalize(normal * dot(v_lightDir, normal) * 2.0 - v_lightDir);
	float spec = pow(max(dot(reflectDir, v_viewDir), 0.0), shininess);
#else
	// bi phone
	vec3 reflectDir = normalize(v_lightDir + v_viewDir);
	float spec = pow(max(dot(reflectDir, normal), 0.0), shininess);
#endif
	vec3 specular = lightColor * (specularStrength * spec);

	vec3 color = u_ambientColor + diffuse + specular;
	FragColor = texture(u_texture0, v_texcoord0) * vec4(color, 1.0);
}
