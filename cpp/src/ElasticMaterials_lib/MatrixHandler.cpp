#include "ElasticMaterials_lib/MatrixHandler.h"

MatrixHandler::MatrixHandler(Camera* camera){
    camera_ = camera;
}

//double MatrixHandler::zoom = 0;
//float MatrixHandler::rotate = 0;
//GLFWwindow* MatrixHandler::window_ = NULL;
//Camera* MatrixHandler::camera_ = NULL;


void MatrixHandler::calculateMatrices(){
    
    M = glm::mat4(1.0f);
    
    glm::mat4 rotate = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),camera_->direction, camera_->up);
    glm::mat4 translate = glm::translate(-camera_->position);
    glm::mat4 view_translate = glm::translate(glm::vec3(
                                                        -camera_->target_->centerOfMass().x,
                                                        0.0f,
                                                        -camera_->target_->centerOfMass().z));
    
    V =  translate * rotate * view_translate;
    MV = V * M;
    P = glm::perspective(camera_->fov, camera_->getAspectRatio(), 0.1f, 400.f);
    MVP = P*V*M;
}

Camera::Camera(GLFWwindow* window, MCS* target){
    target_ = target;
    window_ = window;
    glfwSetScrollCallback(window, scroll_fun);
    scroll_fun(window_,0,0);
}

glm::vec3 Camera::rotation_axis = glm::vec3(0.0f,1.0f,0.0f);
glm::vec3 Camera::position = glm::vec3(0.0f,0.0f,0.0f);
glm::vec3 Camera::direction = glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 Camera::up = glm::vec3(0.0f,1.0f,0.0f);
float Camera::fov = 40;
float Camera::x_pos_incr = 0.0f;
float Camera::y_pos_incr = -35.0f;
MCS* Camera::target_ = NULL;

void Camera::setTarget(MCS* target){
    target_ = target;
}

float Camera::getAspectRatio(){
	int width;
	int height;
    
	glfwGetWindowSize(window_, &width, &height);
    
    float ratio = (float) width / (float) height;
    return ratio;
}

void Camera::scroll_fun(GLFWwindow* window, double x_pos, double y_pos){
    y_pos_incr -= y_pos;
    if(y_pos_incr > 0) y_pos_incr = 0.0f;
    float max_dist = 300.0f;
    if(y_pos_incr < -max_dist) y_pos_incr = -max_dist;
    x_pos_incr += x_pos;
    
    glm::vec3 vertical_position(0.0f,5.0f,0.0f);
    
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), x_pos_incr, up);
    glm::mat4 local_translate = glm::translate(glm::mat4(1.0f), (float) y_pos_incr * glm::vec3(0.0f, 0.0f, -1.0f) + vertical_position);
    
    position = glm::vec3(local_translate * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    direction = glm::vec3(rotate * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));//target_->centerOfMass() - position;
    //std::cout << position << std::endl;
    //std::cout << "Center of mass = " << target_->centerOfMass() << std::endl;
    
}









