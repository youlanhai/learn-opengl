#version 330 core
out vec4 FragColor;
uniform samplerCube u_texture0;
uniform vec3 u_cameraPos;

in vec3 v_position;
in vec3 v_normal;

void main()
{
	float ratio = 1.0 / 1.52; // 1.52 是玻璃的折射率
	vec3 viewDir = normalize(v_position - u_cameraPos);
	// 计算折射方向
	vec3 refractDir = refract(viewDir, normalize(v_normal), ratio);

	FragColor = texture(u_texture0, refractDir);
}
