#include "Drawable.h"

OpenGL_drawable::OpenGL_drawable(){
    vertexArray = GL_FALSE;
    vertexPositionBuffer = GL_FALSE;
    vertexColorBuffer = GL_FALSE;
    elementBuffer = GL_FALSE;

    MVP_loc = -1;
}

OpenGL_drawable::~OpenGL_drawable(){
    // Release memory
    if(vertexPositionBuffer)
        glDeleteBuffers(1, &vertexPositionBuffer);
    if(vertexColorBuffer)
        glDeleteBuffers(1, &vertexColorBuffer);
    if(vertexArray)
        glDeleteVertexArrays(1, &vertexArray);
}
