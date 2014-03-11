#ifndef MATRIX_HANDLER_H
#define MATRIX_HANDLER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>


class MatrixHandler{
public:
	MatrixHandler(GLFWwindow* window);
	void calculateMatrices(float ratio);

    glm::mat4 M;
	glm::mat4 V;
	glm::mat4 P;
	glm::mat4 MV;
	glm::mat4 MVP;
private:
    static void scroll_fun(GLFWwindow* window, double x_pos, double y_pos);

	GLFWwindow* window_;
    static double zoom;
    static float rotate;
};

#endif