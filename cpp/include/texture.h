#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
//#include <GL/glfw.h>

// Load a .BMP file using our custom loader
GLuint loadBMP_custom(const char * imagepath);

// Load a .TGA file using GLFW's own loader
//GLuint loadTGA_glfw(const char * imagepath);

// Load a .DDS file using GLFW's own loader
GLuint loadDDS(const char * imagepath);


#endif