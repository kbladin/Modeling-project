#version 330

// Input data
in vec3 fragColor;
in vec3 fragNormal_worldSpace;
in vec3 vertexPosition_modelspace;
// Ouput data
out vec3 color;

void main(){
	//Light in viewSpace
	vec3 light = vec3(-1,-1,0);
	vec3 n = normalize(fragNormal_worldSpace);

	// Ambient
	float ambientBrightness = 0.2;
	vec3 ambientColor = vec3(1,1,1);
	vec3 ambient = ambientColor * fragColor * ambientBrightness;

	// Diffuse
	float cosTheta = clamp(dot(light, -n),0,1);
	vec3 diffuse = fragColor * cosTheta;

	// Specular
	vec3 r = normalize(reflect(light,n))*1.1;
	vec3 viewVector = vec3(0,0,-1);;
	float cosAlpha = clamp(dot(-r,viewVector),0,1);
	vec3 specularColor = vec3(1,1,1);
	float specularBrightness = 0.5;
	vec3 specular = specularColor * pow(cosAlpha,32) * specularBrightness;


    color = ambient + 0.5*diffuse + specular;
}