#version 330

in vec4 vVertex;
in vec3 vNormal;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform vec3 vLightPosition;

smooth out vec3 vVaryingNormal;
smooth out vec3 vVaryingLightDir;
smooth out vec3 vVaryingTexCoord;

void main(void)  { 

	vVaryingNormal = normalMatrix * vNormal;

	vec4 vPosition4 = mvMatrix * vVertex;
	vec3 vPosition3 = vPosition4.xyz / vPosition4.w;

	vVaryingLightDir = normalize(vLightPosition - vPosition3);
	vVaryingTexCoord = normalize(vVertex.xyz);

	gl_Position = mvpMatrix * vVertex;
}
