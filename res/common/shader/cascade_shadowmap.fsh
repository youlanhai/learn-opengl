#version 330 core

out vec4 FragColor;

in vec2 v_texcoord;
in vec4 v_posInLight;
in vec3 v_posInView;
in vec3 v_normal;

uniform sampler2D u_texture0;

uniform vec3 u_ambientColor;
uniform vec3 lightColor;
uniform vec3 lightDir;

#define MAX_CASCADES 4

uniform sampler2DArray cascadeTexture;
uniform float cascadeSplits[MAX_CASCADES];
uniform mat4 cascadeProjMatrices[MAX_CASCADES];

// 返回0，表示处于阴影中。1表示无阴影
float shadow()
{
	int index = 0;
	for(int i = 0; i < MAX_CASCADES; ++i)
	{
		if(v_posInView.z < cascadeSplits[i])
		{
			index = i;
			break;
		}
	}

	vec4 texcoord = cascadeProjMatrices[index] * vec4(v_posInLight.xyz, 1.0);
	texcoord = texcoord / texcoord.w;
	texcoord = texcoord * 0.5 + 0.5;

	// z值也需要转换到[0, 1]之间
	float currentDepth = texcoord.z;
	texcoord.z = index;

	float depth = texture(cascadeTexture, texcoord.xyz).r;
	return depth + 0.005 > currentDepth ? 1.0 : 0.0;
}

void main()
{
	vec4 albedo = texture(u_texture0, v_texcoord);

	vec3 normal = normalize(v_normal);
	float diff = max(0.0, dot(normal, lightDir));

	vec3 viewDir = normalize(v_posInView);
	vec3 H = normalize(lightDir + viewDir);
	float spec = max(0.0, dot(normal, H));

	float s = shadow();
	vec3 color = u_ambientColor * albedo.rgb + lightColor * albedo.rgb * diff * s;

	FragColor = vec4(color, albedo.a);
}
