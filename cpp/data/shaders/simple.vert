#version 330

// Input
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Output
out vec3 fragColor;
out vec3 fragNormal_worldSpace;
out vec3 vertexPosition_viewSpace;

// Uniforms
uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 M;


void main(){
	gl_Position = MVP*vec4(vertexPosition_modelspace, 1);
    fragColor = vertexColor;
    fragNormal_worldSpace = (M * vec4(vertexNormal_modelspace,1)).xyz;	 //(MV*vec4(vertexPosition_modelspace, 1)).xyz;
    vertexPosition_viewSpace = (M * vec4(vertexPosition_modelspace,1)).xyz;
}