
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

struct Material{
    float reflectance = 1.0f;
    float specularity = 0.5f;
    float shinyness = 30.0f;
    float wetness = 0.0f;
};

class OpenGL_drawable {
public:
    OpenGL_drawable(const MCS* mcs, const Material &material, GLuint programID, GLuint textureID);
    OpenGL_drawable(const CollisionPlane* collision_plane, const Material &material, GLuint programID, GLuint textureID);

    ~OpenGL_drawable();
    
    void setUpBuffers(GLuint textureID);
    void deleteBuffers();
    
    void print() const;
    void updateRuntimeBuffers(const MCS *mcs, MatrixHandler* matrices);
    void updateAllBuffers(const MCS *mcs, Material& material, MatrixHandler* matrices, GLuint textureID);
    void draw();
    
private:
    
    Material material_;
    
    GLuint vertexPositionBuffer;
    GLuint vertexColorBuffer;
    GLuint vertexNormalBuffer;
    GLuint vertexUVBuffer;
    
    GLuint vertexArray;
    GLuint elementBuffer;
    
    //Only need one of these since they are updated anyway
    static GLint MVP_loc_;
    static GLint MV_loc_;
    static GLint V_loc_;
    static GLint M_loc_;
    static GLint lightPos_loc_;
    static GLint lightColor_loc_;
    
    GLint reflectance_loc_;
    GLint specularity_loc_;
    GLint shinyness_loc_;
    GLint wetness_loc_;
    
    GLuint texture_loc_;
    GLuint textureID_;
    
    GLuint programID_;
    
    const glm::vec3* vertex_position_data_;
    const glm::vec3* vertex_normal_data_;
    const glm::vec3* vertex_color_data_;
    const glm::vec2* vertex_UV_data_;
    const IndexedTriangle* element_data_;
    
    int n_verts_;
    int n_elements_;
};

#endif