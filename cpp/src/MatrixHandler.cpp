#include "MatrixHandler.h"

MatrixHandler::MatrixHandler(GLFWwindow* window){
	window_ = window;
    
    glfwSetScrollCallback(window, scroll_fun);
    
}

double MatrixHandler::zoom = 0;
float MatrixHandler::rotate = 0;

void MatrixHandler::calculateMatrices(float ratio){
	double x_mouse;
	double y_mouse;
	int width;
	int height;

	glfwGetCursorPos(window_, &x_mouse, &y_mouse);
	glfwGetWindowSize(window_, &width, &height);

    M = glm::mat4(1.0f);
    glm::mat4 rotate_hor_mat = glm::rotate(glm::mat4(1.0f), rotate, glm::vec3(0.0f,1.0f,0.0f));
    glm::mat4 translate = glm::translate(glm::vec3(0.0f,0.0f, zoom));
    V = translate * rotate_hor_mat;
    MV = V * M;
    P = glm::perspective(45.0f, ratio, 0.1f, 400.f);
    MVP = P*V*M;
}

void MatrixHandler::scroll_fun(GLFWwindow* window, double x_pos, double y_pos){
    zoom -= y_pos;
    rotate -= x_pos;
}