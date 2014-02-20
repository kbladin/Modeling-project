#include <stdlib.h>
#include <stdio.h>
#include <iostream>
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

static void error_callback(int error, const char* description){
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static std::ostream& operator<<(std::ostream& os, const glm::vec3& vec){
    return os << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
}

const float g = 9.82f;

int main(void){

    //Test
    testParticle();
    testConnection();
    testMCS();

    MCS mcs(3,3,3);

    /* INIT GLFW */
    GLFWwindow* window;
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
    float scale = (float) fmax(mcs.N_ROWS, mcs.N_COLS);

    /* INIT SIMULATION */
    int simulations_per_frame = 20;
    float T = 0.1f * 1.0f/(60.0f*simulations_per_frame);
    float current_time;

    //Init gl points
    glEnable( GL_POINT_SMOOTH );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glPointSize( 15.0 );

    //Init gl lines
    glEnable( GL_LINE_SMOOTH );


    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float ratio = width / (float) height;
    glfwSetCursorPos(window, 0,0);

    while (!glfwWindowShouldClose(window)){
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        for (int i = 0; i < simulations_per_frame; ++i)
        {
            
            // Moving one mass 
            double x_mouse;
            double y_mouse;

            //glfwGetCursorPos(window, &x_mouse, &y_mouse);
            //positions[0][write_buffer] = glm::vec2(float(x_mouse-0.5*width)*2*scale/height, -float(y_mouse-0.5*height)*2*scale/height);
            //velocities[0][write_buffer] = glm::vec2(0.0f, 0.0f);

            float scalex = scale*ratio;

            //Calculate acceleration, velocity and position
            // OBS!!!! NOT UPDATING MASS 1 HERE NOW
            /*
            for (int mass_index = 0; mass_index < mcsN_MASSES; ++mass_index){

                float friction = 1.0f;
                //Check collision
                if (p[1] < -scale){
                    p[1] = -scale;
                    v[1] = -v[1];
                    v[0] *= (1-friction);
                }
                else if (p[1] > scale){
                    p[1] = scale;
                    v[1] = -v[1];
                    v[0] *= (1-friction);
                }

                if (p[0] < -scalex){
                    p[0] = -scalex;
                    v[0] = -v[0];
                    v[1] *= (1-friction);
                }
                else if (p[0] > scalex){
                    p[0] = scalex;
                    v[0] = -v[0];
                    v[1] *= (1-friction);
                }
                
            }
            */
            
            mcs.update(T);
            Particle::swapBuffers();
        }

        /* DRAW */
        
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio * scale, ratio * scale, -1.f * scale, 1.f * scale, 1.f * scale, -1.f * scale);
        glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
        //Draw masses
        glBegin(GL_POINTS);
        glColor3f(1.f, 0.f, 0.f);
        for (int i = 0; i < mcs.getNumberOfParticles(); ++i){
            //glVertex3f(positions[i][read_buffer][0],
            //           positions[i][read_buffer][1], 0.f);
        }
        glEnd();

        //Draw connections
        glBegin(GL_LINES);
        glColor3f(0.0f, 1.f, 0.f);
        
        for (int i = 0; i < mcs.getNumberOfConnections(); ++i){
            const Connection& c = mcs.getConnection(i);
            const glm::vec3& p1 = c.getParticle_1().readPosition();
            const glm::vec3& p2 = c.getParticle_2().readPosition();
            const glm::vec3& delta_p = p1 - p2;
            
            float r = glm::abs(glm::length(delta_p)-c.getConnectionLength());

            glColor3f(r,1-r,0.0f);
            glVertex3f(p1[0], p1[1], p1[2]);
            glVertex3f(p2[0], p2[1], p2[2]);
        }
        glEnd();



        //Swap draw buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /* CLEAN UP GLFW */
    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}


/*


bool myInitFun();
void myDrawFun();
void myPreSyncFun();
void myCleanUpFun();
 
//global vars
GLuint vertexArray = GL_FALSE;
GLuint vertexPositionBuffer = GL_FALSE;
GLuint vertexColorBuffer = GL_FALSE;

GLint MVP_loc = -1;

GLuint programID;

GLFWwindow* window;
 
int main( int argc, char* argv[] )
{

    // Init
    if (!myInitFun()){
        return -1;
    }

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        
        myDrawFun();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    myCleanUpFun();
    // Exit program
}


bool myInitFun()
{
    // Init glfw
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

    // Initialize GLEW (Create OpenGL context)
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return false;
    }

    // Current OpenGL version    
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // Vertexdata
    const GLfloat vertex_position_data[] = { 
        -0.5f, -0.5f, 0.0f,
         0.0f, 0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };
    // Colordata
    const GLfloat vertex_color_data[] = { 
        1.0f, 0.0f, 0.0f, //red
        0.0f, 1.0f, 0.0f, //green
        0.0f, 0.0f, 1.0f //blue
    };

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_position_data), vertex_position_data, GL_STATIC_DRAW);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_color_data), vertex_color_data, GL_STATIC_DRAW);
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


void myDrawFun()
{
    float speed = 50.0f;

    glm::mat4 MVP = glm::rotate(glm::mat4(1), speed * (float) glfwGetTime(), glm::vec3(0,0,1));
 
    //BIND SHADER HERE
    glUseProgram(programID);
 
    glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, &MVP[0][0]);
 
    glBindVertexArray(vertexArray);
 
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 3);
 
    //unbind
    glBindVertexArray(0);
    
    //UNBIND SHADER HERE
    glUseProgram(0);
}
 
void myPreSyncFun()
{

}
 
void myCleanUpFun()
{
    // Terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);

    // Release memory
    if(vertexPositionBuffer)
        glDeleteBuffers(1, &vertexPositionBuffer);
    if(vertexColorBuffer)
        glDeleteBuffers(1, &vertexColorBuffer);
    if(vertexArray)
        glDeleteVertexArrays(1, &vertexArray);
}





int main(void)
{





    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }




    //std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;


    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.6f, 0.f);
        glEnd();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

*/
