
#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cassert>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <shader.h>

#include "MCS.h"


class OpenGL_drawable {
public:
    OpenGL_drawable();
    ~OpenGL_drawable();

    void deleteBuffers();

    GLuint vertexArray;
    GLuint vertexPositionBuffer;
    GLuint vertexColorBuffer;
    GLuint elementBuffer;

    GLuint vertexNormalBuffer;

    GLint MVP_loc;
    GLint MV_loc;
    GLint V_loc;
    GLint M_loc;
    GLint lightPos_loc;
    GLint lightColor_loc;

    GLuint programID;
    // Vertex color data
    std::vector<glm::vec3> vertex_color_data;

    void print() const;
};


class OpenGL_Drawer{
public:
    bool add(MCS& mcs);
    bool draw();
    
    float ratio;

    std::vector<MCS*> vecMCS;
    std::vector<OpenGL_drawable> vecDrawable;
private:
    
    

};

#endif