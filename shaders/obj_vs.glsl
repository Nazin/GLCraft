// GLSL 3.30
#version 330 core

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

in vec3 inVertex;
in vec3 inNormal;

out vec3 fragmentEye;
out vec3 fragmentNormal;

void main(void) {

	vec4 vertex = vec4(inVertex,1.0);

	fragmentEye = -normalize((modelViewMatrix*vertex).xyz);
	fragmentNormal = normalize((modelViewMatrix*vec4(inNormal, 0.0)).xyz);

	gl_Position = modelViewProjectionMatrix*vertex;
}
