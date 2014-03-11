#include "MatrixHandler.h"

MatrixHandler::MatrixHandler(GLFWwindow* window){
	window_ = window;

	double x_mouse;
	double y_mouse;
	int width;
	int height;

	glfwGetCursorPos(window_, &x_mouse, &y_mouse);
	glfwGetWindowSize(window_, &width, &height);

	//previous_x_pos_ = (x_mouse/width-0.5)*2;
	//previous_x_pos_ = -(y_mouse/height-0.5)*2;
}

void MatrixHandler::calculateMatrices(float ratio){

	double x_mouse;
	double y_mouse;
	int width;
	int height;

	glfwGetCursorPos(window_, &x_mouse, &y_mouse);
	glfwGetWindowSize(window_, &width, &height);

	float x_pos = (x_mouse/width-0.5)*2 + 1;
	float y_pos = -(y_mouse/height-0.5)*2;

    float speed = 250;
    M = glm::mat4(1.0f);
    glm::mat4 rotate_hor_mat = glm::rotate(glm::mat4(1.0f), x_pos * speed, glm::vec3(0.0f,1.0f,0.0f));
    glm::mat4 translate = glm::translate(glm::vec3(0.0f,0.0f, -(y_pos+0.5) * speed));
    V = translate * rotate_hor_mat;
    MV = V * M;
    P = glm::perspective(45.0f, ratio, 0.1f, 400.f);
    MVP = P*V*M;
}

