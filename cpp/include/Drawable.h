
#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>


class OpenGL_drawable {
public:
    OpenGL_drawable();

    ~OpenGL_drawable();

    GLuint vertexArray;
    GLuint vertexPositionBuffer;
    GLuint vertexColorBuffer;
    GLuint elementBuffer;

    GLint MVP_loc;

    GLuint programID;
    // Vertex color data
    std::vector<glm::vec3> vertex_color_data;    
};

#endif