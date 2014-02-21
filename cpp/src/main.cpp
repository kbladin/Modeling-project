#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <shader.h>
#include "connection2massindices.h"
#include "Particle.h"
#include "Connection.h"
#include "test.h"

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void initGLFW();
bool initOpenGL();
void cleanUpGLFW();
void cleanUpOpenGl();
void draw();

static std::ostream& operator<<(std::ostream& os, const glm::vec3& vec){
    return os << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
}

int width, height;
float ratio;
float scale;

GLFWwindow* window;


// THIS IS USED ONLY FOR MODERN OPENGL
GLuint vertexArray = GL_FALSE;
GLuint vertexPositionBuffer = GL_FALSE;
GLuint vertexColorBuffer = GL_FALSE;

GLint MVP_loc = -1;

GLuint programID;

// Vertexdata
std::vector<glm::vec3> vertex_position_data;
// Colordata
std::vector<glm::vec3> vertex_color_data;

MCS mcs = MCS(3,3,3);

int main(void){

    //Test
    //testParticle();
    testConnection();
    testMCS();


    initGLFW();
    initOpenGL();
    scale = 11;// (float) fmax(N_ROWS,N_COLS);
    ratio = width / (float) height;

    
    mcs.addRotation(glm::vec3(1.0,0.5,0.0),25.0f);
    mcs.setAvgPosition(glm::vec3(-10,0,-30));
    mcs.setAvgVelocity(glm::vec3(5,5,0));

    // INIT SIMULATION 
    int simulations_per_frame = 40;
    float T = 1.0f/(60.0f*simulations_per_frame);

    float current_time;

    while (!glfwWindowShouldClose(window)){
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        for (int i = 0; i < simulations_per_frame; ++i)
        {   
            // Moving one mass 
            double x_mouse;
            double y_mouse;

            glfwGetCursorPos(window, &x_mouse, &y_mouse);
            glm::vec2 pos2d = glm::vec2(float(x_mouse-0.5*width)*2*scale/height, -float(y_mouse-0.5*height)*2*scale/height);
            //mcs.setAvgPosition(glm::vec3(pos2d[0],pos2d[1],10));
            //mcs.getParticle(0).writePosition(glm::vec3(pos2d[0],pos2d[1],10.0f));
            //mcs.getParticle(5).writePosition(glm::vec3(pos2d[0],pos2d[1],16.0f));
            //velocities[0][write_buffer] = glm::vec2(0.0f, 0.0f);

            float scalex = scale*ratio;
            mcs.update(T);
        }

        // DRAW
        draw();

        //Swap draw buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    cleanUpGLFW();
    cleanUpOpenGl();
}

static void error_callback(int error, const char* description){
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void initGLFW(){
    
    // INITIALISATION OF GLFW FOR GLBEGIN
/*
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window){
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);



    glfwGetFramebufferSize(window, &width, &height);
    
    glfwSetCursorPos(window, 0,0);
*/

    // INITIALISATION OF GLFW FOR MODERN OPENGL

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    // We want the newest version of OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

}

bool initOpenGL(){
    
    // INITIALISATION OF OLD OPENGL

    //Init gl points
    glEnable( GL_POINT_SMOOTH );
    //glEnable( GL_BLEND );
    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glPointSize( 5.0 );

    //Init gl lines
    //glEnable( GL_LINE_SMOOTH );
    


    // INITIALISATION OF MODERN OPENGL

    // Initialize GLEW (Create OpenGL context)
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return false;
    }

    // Current OpenGL version    
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    for (int i = 0; i < mcs.getNumberOfParticles(); ++i){
        vertex_position_data.push_back(mcs.particles.positions[i]);
        vertex_color_data.push_back(glm::vec3(1.f, 0.f, 0.f));
    }

    std::cout 
        << vertex_position_data[0] << std::endl 
        << vertex_position_data[1] << std::endl
        << vertex_position_data[2] << std::endl;

    vertex_position_data[0] = glm::vec3(50.f, 50.f, 1);
    vertex_position_data[1] = glm::vec3(0.f, 0.f, 1);
    vertex_position_data[2] = glm::vec3(0.f, 50.f, 1);


    //generate the VAO
    glGenVertexArrays(1, &vertexArray);

    // Create and compile the shader
    programID = LoadShaders( "data/shaders/simple.vert", "data/shaders/simple.frag" );

    // Bind the VAO (will contain one vertex position buffer and one vertex color buffer)
    glBindVertexArray(vertexArray);
 
    //generate VBO for vertex positions
    glGenBuffers(1, &vertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
    //upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_position_data.size(), &vertex_position_data[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        reinterpret_cast<void*>(0) // array buffer offset
    );
 
    //generate VBO for vertex colors
    glGenBuffers(1, &vertexColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer);
    //upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_color_data.size(), &vertex_color_data[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        reinterpret_cast<void*>(0) // array buffer offset
    );
    
    // Unbind the current VAO
    glBindVertexArray(0);
 
    //BIND SHADER HERE
    //glUseProgram(programID);
 
    //GET UNIFORM LOCATION FOR MVP MATRIX HERE
    //Matrix_Loc = sgct::ShaderManager::instance()->getShaderProgram( "xform").getUniformLocation( "MVP" );
    MVP_loc = glGetUniformLocation( programID, "MVP");


    //UNBIND SHADER HERE
    // ------

    return true;

}

void draw(){

    // DRAW OLD OPENGL
/*
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float zNear = 0.1f;
    float zFar = 255.0f;

    glOrtho(-ratio * scale, ratio * scale, -1.f * scale, 1.f * scale, zNear, -zFar);
    glTranslatef(0.f,0.f,0.f);
    //glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
    //Draw masses

    glBegin(GL_POINTS);
    glColor3f(1.f, 0.f, 0.f);
    for (int i = 0; i < mcs.getNumberOfParticles(); ++i){
        break;
        glm::vec3 pos = mcs.particles.positions[i];
        glVertex3f(pos[0],pos[1],pos[2]);
    }
    glEnd();


    //Draw connections
    glBegin(GL_LINES);
    for (int i = 0; i < mcs.getNumberOfConnections(); ++i){
        int index_p1 = mcs.connections.particle1[i];
        int index_p2 = mcs.connections.particle2[i];
        const glm::vec3& p1 = mcs.particles.positions[index_p1];
        const glm::vec3& p2 = mcs.particles.positions[index_p2];
        const glm::vec3& delta_p = p1 - p2;
        
        float r = 5.0f*glm::abs(glm::length(delta_p)-mcs.connections.lengths[i]);
        glColor3f(r,1-r,0.0f);
        glVertex3f(p1[0], p1[1], p1[2]);
        glVertex3f(p2[0], p2[1], p2[2]);
    }
    glEnd();    
*/


    //DRAW WITH MODERN OPENGL

    for (int i = 0; i < mcs.getNumberOfParticles(); ++i){
        vertex_position_data[i] = mcs.particles.positions[i];
    }




    // THIS IS NOR SUPER (SENDING DATA TO GPU EVERY FRAME)
    // Bind the VAO (will contain one vertex position buffer and one vertex color buffer)
    glBindVertexArray(vertexArray);
 
    //generate VBO for vertex positions
    glGenBuffers(1, &vertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
    //upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_position_data.size(), &vertex_position_data[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        reinterpret_cast<void*>(0) // array buffer offset
    );
 
    //generate VBO for vertex colors
    glGenBuffers(1, &vertexColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer);
    //upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_color_data.size(), &vertex_color_data[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        reinterpret_cast<void*>(0) // array buffer offset
    );
    
    // Unbind the current VAO
    glBindVertexArray(0);





    float speed = 50.0f;

    //glm::mat4 MVP = glm::ortho(-10.f, 10.f, -10.f, 10.f);//glm::rotate(glm::mat4(1), speed * (float) glfwGetTime(), glm::vec3(0,0,1));
    glm::mat4 MVP = glm::perspective(45.0f, ratio, 0.1f, 100.f);
 
    //BIND SHADER HERE
    glUseProgram(programID);
 
    glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, &MVP[0][0]);
 
    glBindVertexArray(vertexArray);
 
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the triangle !
    glDrawArrays(GL_POINTS, 0, vertex_position_data.size());
 
    //unbind
    glBindVertexArray(0);
    
    //UNBIND SHADER HERE
    glUseProgram(0);


}

void cleanUpOpenGl()
{
    // Release memory
    if(vertexPositionBuffer)
        glDeleteBuffers(1, &vertexPositionBuffer);
    if(vertexColorBuffer)
        glDeleteBuffers(1, &vertexColorBuffer);
    if(vertexArray)
        glDeleteVertexArrays(1, &vertexArray);
}

void cleanUpGLFW()
{
    // Terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

