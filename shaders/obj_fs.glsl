// GLSL 3.30
#version 330 core

in vec3 fragmentNormal;
in vec3 fragmentEye;

out vec4 fragColor;

void main(void) {

	float diffuseIntensity;
	float specularItensity;

	vec3 light;

	vec3 normal;
	vec3 eye;
	
	vec3 reflection;

	light = normalize(vec3(5.0, 5.0, 10.0));

	normal = fragmentNormal;
	eye = fragmentEye;
	
	diffuseIntensity = max(dot(normal, light), 0.0);
	
	reflection = normalize(reflect(-light, normal));
	specularItensity = pow(max(dot(reflection, eye), 0.0), 50.0 );

	fragColor = vec4(0.0, 0.0, 0.0, 1.0) + vec4(1.0, 0.0, 0.0, 1.0)*diffuseIntensity + vec4(1.0, 0.9, 0.9, 1.0)*specularItensity;
}
