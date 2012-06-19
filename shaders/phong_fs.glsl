#version 330

out vec4 vFragColor;

uniform samplerCube cubeMap;

smooth in vec3 vVaryingNormal;
smooth in vec3 vVaryingLightDir;
smooth in vec3 vVaryingTexCoord;

void main(void) { 

	float diff = max(0.0, dot(normalize(vVaryingNormal), normalize(vVaryingLightDir)));
    
	vFragColor = diff * vec4(0.2, 0.2, 0.2, 1.0); //diffuse
	vFragColor += vec4(0.1, 0.1, 0.1, 1.0); //ambient

	vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir), normalize(vVaryingNormal)));
	float spec = max(0.0, dot(normalize(vVaryingNormal), vReflection));

	if (diff != 0) {
		float fSpec = pow(spec, 128.0);
		vFragColor.rgb += vec3(fSpec, fSpec, fSpec);
	}

	vFragColor += texture(cubeMap, vVaryingTexCoord);
}
    