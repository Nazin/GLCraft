#version 330

out vec4 vFragColor;

uniform samplerCube cubeMap;

smooth in vec3 fragmentNormal;
smooth in vec3 vVaryingTexCoord;

void main(void) { 

	float intensity;

	intensity = max(dot(fragmentNormal, vec3(0.0, 0.0, 0.35)), 0.0);

	vFragColor = texture(cubeMap, vVaryingTexCoord)*intensity;
}
