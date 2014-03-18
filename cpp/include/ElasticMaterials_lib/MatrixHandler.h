#ifndef MATRIX_HANDLER_H
#define MATRIX_HANDLER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <ElasticMaterials_lib.h>

#include "debugtools.h"

class Camera;

class MatrixHandler{
public:
	MatrixHandler(Camera* camera);
	void calculateMatrices();

    glm::mat4 M;
	glm::mat4 V;
	glm::mat4 P;
	glm::mat4 MV;
	glm::mat4 MVP;
private:
    Camera* camera_;
};

class Camera{
public:
    Camera(GLFWwindow* window, MCS* target);

    void setTarget(MCS* target);
    
    float getAspectRatio();
    
    static glm::vec3 rotation_axis;
    static glm::vec3 position;
    static glm::vec3 direction;
    static glm::vec3 up;
    static float fov;

    static MCS* target_;

private:
    static void scroll_fun(GLFWwindow* window, double x_pos, double y_pos);
	
    static float x_pos_incr;
    static float y_pos_incr;
    
    GLFWwindow* window_;
};

#endif