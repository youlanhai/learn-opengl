#version 330 core

out vec4 FragColor;

in vec2 v_texcoord;
in vec4 v_posInLight;
in vec3 v_posInWorld;
in vec3 v_normal;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform vec3 u_cameraPos;
uniform vec3 u_ambientColor;
uniform vec3 lightColor;
uniform vec3 lightDir;

float shadow()
{
	vec3 texcoord = v_posInLight.xyz / v_posInLight.w;
	texcoord = texcoord * 0.5 + 0.5;

	// z值也需要转换到[0, 1]之间
	float currentDepth = texcoord.z;
	float depth = texture(u_texture1, texcoord.xy).r;

	return depth + 0.005 > currentDepth ? 1.0 : 0.0;
}

void main()
{
	vec4 albedo = texture(u_texture0, v_texcoord);

	vec3 normal = normalize(v_normal);
	float diff = max(0.0, dot(normal, lightDir));

	vec3 viewDir = normalize(v_posInWorld - u_cameraPos);
	vec3 H = normalize(lightDir + viewDir);
	float spec = max(0.0, dot(normal, H));

	float s = shadow();
	vec3 color = u_ambientColor * albedo.rgb + lightColor * albedo.rgb * diff * s;

	FragColor = vec4(color, albedo.a);
}
