#version 330

in vec4 vVertex;

uniform mat4 mvpMatrix;

smooth out vec3 fragmentNormal;
smooth out vec3 vVaryingTexCoord;

void main(void) {

	vVaryingTexCoord = normalize(vVertex.xyz);

	gl_Position = mvpMatrix * vVertex;

	fragmentNormal = (mvpMatrix*vVertex).xyz;
}
