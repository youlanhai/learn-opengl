
uniform sampler2D u_texture0; // main texture
uniform sampler2D u_texture1; // shadow map
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform vec3 cameraPos;
uniform float specularStrength;
uniform float shininess;
uniform vec2 texelSize; // 1 / textureSize

varying vec2 v_texcoord;
varying vec3 v_normal;
varying vec3 v_posInWorld;
varying vec4 v_posInLightSpace;

float shadowMapping()
{
	// 手动执行透视除法
	vec3 projCoord = v_posInLightSpace.xyz / v_posInLightSpace.w;

	// 将坐标从[-1, 1]转换到[0, 1]
	projCoord = projCoord * 0.5 + 0.5;

	float currentDepth = projCoord.z;
	float closestDepth = texture2D(u_texture1, projCoord.xy).r;

	// 如果不考虑阴影失真，到这里就可以返回了。
	// 如果当前的深度值更大，说明当前的点就在阴影中
	// return currentDepth > closestDepth;

	//float diffuse = dot(normalize(v_normal), normalize(lightDir));
	float bias = 0.0001;// max(0.05 * (1.0 - diffuse), 0.005);

	float shadow = 0.0;
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture2D(u_texture1, projCoord.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}

void main()
{
	float diff = max(dot(v_normal, lightDir), 0.0);

	vec3 viewDir = normalize(cameraPos - v_posInWorld);
	vec3 halfDir = normalize(viewDir + lightDir);
	float spec = max(dot(v_normal, halfDir), 0.0);
	spec = pow(spec, shininess) * specularStrength;

	vec3 color = ambientColor + lightColor * ((diff + spec) * (1.0 - shadowMapping()));

	gl_FragColor = texture2D(u_texture0, v_texcoord) * vec4(color, 1.0);
}