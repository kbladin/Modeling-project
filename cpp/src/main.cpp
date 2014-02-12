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

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

const int N_ROWS = 40;
const int N_COLS = 40;

const int N_TYPE1 = (N_ROWS-1)*N_COLS; // |
const int N_TYPE2 = (N_ROWS-1)*(N_COLS-1); // /
const int N_TYPE3 = N_ROWS*(N_COLS-1); // _
const int N_TYPE4 = N_TYPE2; // \.

const int N_MASSES = N_ROWS*N_COLS;
const int N_CONNECTIONS = N_TYPE1+N_TYPE2+N_TYPE3+N_TYPE4;

float masses[N_MASSES];
glm::vec2 positions[N_MASSES][2];
glm::vec2 velocities[N_MASSES][2];
glm::vec2 forces[N_MASSES];

int connected_masses[N_CONNECTIONS][2];
float spring_constants[N_CONNECTIONS];
float damper_constants[N_CONNECTIONS];
float spring_lengths[N_CONNECTIONS];

const float g = 9.82f;



int main(void)
{
    /* INIT GLFW */
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    float scale = 40.f;

    /* INIT SIMULATION */

    // Set start values to masses, positions velocities and forces
    for (int i = 0; i < N_MASSES; ++i)
    {
        masses[i] = 1.0f;
        int row = i%N_COLS;
        int col = floor(i/N_COLS);
        positions[i][0] = glm::vec2(row,col);
        positions[i][1] = glm::vec2(0,0);
        velocities[i][0] = glm::vec2(0,0);
        velocities[i][1] = glm::vec2(0,0);
        forces[i] = glm::vec2(0,0);
    }
    positions[0][0] = glm::vec2(0.0f,-1.0f);
    velocities[0][0] = glm::vec2(0.0f,-100.0f);


    // Set values for springs, dampers, and lengths
    for (int i = 0; i < N_CONNECTIONS; ++i)
    {
        spring_constants[i] = 2000.0f;
        damper_constants[i] = 5.0f;
        spring_lengths[i] = 1.0f;
    }

    // Set type_2 connections to legnth sqrt(2)
    for (int i = N_TYPE1; i < N_TYPE1+N_TYPE2; ++i)
    {
        spring_lengths[i] = sqrt(2);
    }

    // Set type_4 connections to legnth sqrt(2)
    for (int i = N_TYPE1+N_TYPE2+N_TYPE3; i < N_CONNECTIONS; ++i)
    {
        spring_lengths[i] = sqrt(2);
    }

    /*
    Mass indices:
    2---------3
    |  \   /  |
    |    X    |
    |  /   \  |
    0---------1

    Connection indices:
    #----4----#
    |  \  2/  |
    0    X    1
    |  /   \5 |
    #----3----#
    */

/*
    // Hard coded connections
    connected_masses[0][0] = 0;
    connected_masses[0][1] = 2;
    connected_masses[1][0] = 1;
    connected_masses[1][1] = 3;
    connected_masses[2][0] = 0;
    connected_masses[2][1] = 3;
    connected_masses[3][0] = 0;
    connected_masses[3][1] = 1;
    connected_masses[4][0] = 2;
    connected_masses[4][1] = 3;
    connected_masses[5][0] = 1;
    connected_masses[5][1] = 2;
*/

    // Calculate connections
    for (int i = 0; i < N_CONNECTIONS; ++i)
    {
        connection2massIndices(i, connected_masses[i][0], connected_masses[i][1], N_ROWS, N_COLS);
    }

    int read_buffer = 0;
    int write_buffer = 1;

    float T = 0.01f;
    float current_time;

    //Init gl points
    glEnable( GL_POINT_SMOOTH );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glPointSize( 15.0 );

    //Init gl lines
    glEnable( GL_LINE_SMOOTH );


    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float ratio = width / (float) height;

        /* SIMULATION */
        for (int connection_index = 0; connection_index < N_CONNECTIONS; ++connection_index)
        {
            //connection properties
            float k = spring_constants[connection_index];
            float b = damper_constants[connection_index];
            float l = spring_lengths[connection_index];

            //sosition
            int mass_index1 = connected_masses[connection_index][0];
            int mass_index2 = connected_masses[connection_index][1];
            glm::vec2 p1 = positions[mass_index1][read_buffer];
            glm::vec2 p2 = positions[mass_index2][read_buffer];
            glm::vec2 delta_p = p1 - p2;
            glm::vec2 delta_p_hat = glm::normalize(delta_p);//Not tested

            //velocities
            glm::vec2 v1 = velocities[mass_index1][read_buffer];
            glm::vec2 v2 = velocities[mass_index2][read_buffer];
            glm::vec2 delta_v = v1 - v2;

            //calculate force from connection
            glm::vec2 force_from_connection = ( -k*(glm::length(delta_p) - l) - b*(delta_v*delta_p_hat) )*delta_p_hat;
            forces[mass_index1] += force_from_connection;
            forces[mass_index2] -= force_from_connection;
        }

        // Moving one mass 
        double x_mouse;
        double y_mouse;

        glfwGetCursorPos(window, &x_mouse, &y_mouse);
        positions[0][write_buffer] = glm::vec2(float(x_mouse-0.5*width)*2*scale/height, -float(y_mouse-0.5*height)*2*scale/height);

        float scalex = scale*ratio;

        //Calculate acceleration, velocity and position
        for (int mass_index = 1; mass_index < N_MASSES; ++mass_index) // OBS!!!! NOT UPDATING MASS 1 HERE NOW
        {
            glm::vec2 a = forces[mass_index]/masses[mass_index];// - glm::vec2(0.f,1.f)*g;
            glm::vec2 v = velocities[mass_index][read_buffer] + a*T;
            glm::vec2 p = positions[mass_index][read_buffer] + v*T;

            //Check collision
            if (p[1] < -scale)
            {
                p[1] = -scale;
                v[1] = -v[1];
            }
            else if (p[1] > scale)
            {
                p[1] = scale;
                v[1] = -v[1];   
            }


            if (p[0] < -scalex)
            {
                p[0] = -scalex;
                v[0] = -v[0];
            }
            else if (p[0] > scalex)
            {
                p[0] = scalex;
                v[0] = -v[0];
            }

            //Store information in backbuffer
            velocities[mass_index][write_buffer] = v;
            positions[mass_index][write_buffer] = p;

            //reset force
            forces[mass_index] = glm::vec2(0.0f, 0.0f);
        }

        /* DRAW */
        
        //int width, height;
        //glfwGetFramebufferSize(window, &width, &height);
        
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio * scale, ratio * scale, -1.f * scale, 1.f * scale, 1.f * scale, -1.f * scale);

        //Draw masses
        glBegin(GL_POINTS);
        glColor3f(1.f, 0.f, 0.f);
        for (int i = 0; i < N_MASSES; ++i)
        {
            glVertex3f(positions[i][read_buffer][0],
                       positions[i][read_buffer][1], 0.f);
        }
        glEnd();

        //Draw connections
        glBegin(GL_LINES);
        glColor3f(0.f, 1.f, 0.f);
        for (int i = 0; i < N_CONNECTIONS; ++i)
        {
            glVertex3f(positions[connected_masses[i][0]][read_buffer][0],
                       positions[connected_masses[i][0]][read_buffer][1], 0.f);
            glVertex3f(positions[connected_masses[i][1]][read_buffer][0],
                       positions[connected_masses[i][1]][read_buffer][1], 0.f);
        }
        glEnd();

        //Swap simulation buffers
        read_buffer = (read_buffer+1)%2;
        write_buffer = (write_buffer+1)%2;

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
