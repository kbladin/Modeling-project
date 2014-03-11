#ifndef MATRIX_HANDLER_H
#define MATRIX_HANDLER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>


class MatrixHandler{
public:
	glm::mat4 M;
	glm::mat4 V;
	glm::mat4 P;
	glm::mat4 MV;
	glm::mat4 MVP;

	MatrixHandler(GLFWwindow* window);
	void calculateMatrices(float ratio);

private:
	GLFWwindow* window_;
	//float previous_x_pos_;
	//float previous_y_pos_;
};

#endif