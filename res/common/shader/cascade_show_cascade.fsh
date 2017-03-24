#version 330 core

out vec4 FragColor;
in vec3 v_normal; // view space
in vec3 v_posInView; // view space
in vec3 v_posInLight;


uniform sampler2DArray cascadeTexture;
uniform vec3 u_ambientColor;
uniform vec3 lightDir; // view space
uniform vec3 lightColor;

#define MAX_CASCADES 4
uniform float cascadeSplits[MAX_CASCADES];
uniform mat4 cascadeProjMatrices[MAX_CASCADES];

vec3 ShadowColors[MAX_CASCADES] = vec3[MAX_CASCADES](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0)
);

vec3 shadow()
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

	vec4 texcoord = cascadeProjMatrices[index] * vec4(v_posInLight, 1.0);
	texcoord = texcoord / texcoord.w * 0.5 + 0.5;

	float currentDepth = texcoord.z;
	texcoord.z = index;

	float textureDepth = texture(cascadeTexture, texcoord.xyz).r;
	float s = textureDepth + 0.005 < currentDepth ? 0.2 : 1.0;
	return ShadowColors[index] * s;
}

void main()
{
	vec3 sColor = shadow();

	float diff = max(0.0, dot(lightDir, normalize(v_normal)));
	vec3 color = u_ambientColor + lightColor * sColor * diff;

	FragColor = vec4(color, 1.0);
}
