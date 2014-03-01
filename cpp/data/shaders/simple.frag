#version 330

// Input data
in vec3 fragColor;
in vec3 fragPosition_worldSpace;
in vec3 fragNormal_viewSpace;
in vec3 viewDirectionToVertex_viewSpace;
in vec3 lightDirectionToFragment_viewSpace;

// Uniforms
uniform vec3 lightPos_worldSpace;
uniform vec3 lightColor;

// Ouput data
out vec3 color;

void main(){
	vec3 materialDiffuseColor = fragColor;

	float lightIntensity = 400;

	vec3 n = normalize(fragNormal_viewSpace);
	vec3 e = normalize(viewDirectionToVertex_viewSpace); // Eye vector (away from the camera)
	vec3 l = normalize(lightDirectionToFragment_viewSpace);
	vec3 r = reflect(l,n);

	float distanceToLight = length(lightPos_worldSpace - fragPosition_worldSpace);
	float distanceSquare = pow(distanceToLight,2);
	float invDistSquare =  1.0f/(distanceSquare);

	// Ambient light
	float ambientBrightness = 0.1;
	vec3 ambientColor = vec3(1,1,1);
	vec3 ambient = ambientColor * fragColor * ambientBrightness;
	
	//Diffuse light 
	float cosTheta = clamp(dot(-n, l), 0, 1);
	vec3 diffuse = materialDiffuseColor * lightColor * cosTheta * invDistSquare * lightIntensity;

	//Specular light
	float specularity = 0.7;
	float shinyNess = 30;
	float wetness = 0.2;
	float cosAlpha = clamp( dot( e,-r ), 0,1 ) * (1 + wetness);
	vec3 specular = lightColor * clamp(pow(cosAlpha, shinyNess),0,1) * invDistSquare * lightIntensity * specularity;

	// Total light
	color = ambient + diffuse + specular;
}