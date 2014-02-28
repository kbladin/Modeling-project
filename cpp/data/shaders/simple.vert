#version 330

// Input
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Uniforms
uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 M;
uniform mat4 V;


uniform vec3 lightPos_worldSpace;

// Output
out vec3 fragColor;
out vec3 fragPosition_worldSpace;
out vec2 UV;
out vec3 viewDirectionToVertex_viewSpace;
out vec3 fragNormal_viewSpace;

out vec3 lightDirectionToFragment_viewSpace;

void main(){
    fragColor = vertexColor;
	fragPosition_worldSpace = (M * vec4(vertexPosition_modelspace, 1.0)).xyz;

	vec3 vertexPosition_viewSpace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
	viewDirectionToVertex_viewSpace = vec3(0,0,0) + vertexPosition_viewSpace;

	vec3 lightPosition_viewSpace = ( V * vec4(lightPos_worldSpace,1)).xyz;
	lightDirectionToFragment_viewSpace = viewDirectionToVertex_viewSpace - lightPosition_viewSpace;

	fragNormal_viewSpace  = ( V * M * vec4(vertexNormal_modelspace,0)).xyz;

	gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
}