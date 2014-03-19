
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
#include "texture.h"


#include "ElasticMaterials_lib.h"


class OpenGL_drawable {
public:
    OpenGL_drawable(const MCS* mcs);
    ~OpenGL_drawable();
    
    void setUpBuffers();
    void deleteBuffers();
    
    void print() const;
    void updateBuffers(const MCS *mcs, MatrixHandler* matrices);
    void draw();
    
private:
    GLuint vertexPositionBuffer;
    GLuint vertexColorBuffer;
    GLuint vertexNormalBuffer;
    GLuint vertexUVBuffer;
    
    GLuint vertexArray;
    GLuint elementBuffer;
    
    //Only need one of these since they are updated anyway
    static GLint MVP_loc;
    static GLint MV_loc;
    static GLint V_loc;
    static GLint M_loc;
    static GLint lightPos_loc;
    static GLint lightColor_loc;
    
    GLuint texture_loc;
    
    GLuint textureID;
    GLuint programID;
    
    const glm::vec3* vertex_position_data_;
    const glm::vec3* vertex_normal_data_;
    const glm::vec3* vertex_color_data_;
    const glm::vec2* vertex_UV_data_;
    const IndexedTriangle* element_data_;
    
    int n_verts_;
    int n_elements_;
};

#endif