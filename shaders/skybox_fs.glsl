#version 330

out vec4 vFragColor;

uniform samplerCube cubeMap;

smooth in vec3 vVaryingTexCoord;

void main(void) { 
	vFragColor = texture(cubeMap, vVaryingTexCoord);
}
