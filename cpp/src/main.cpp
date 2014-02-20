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
int initOpenGL();
void cleanUpGLFW();
void cleanUpOpenGl();
void draw();

static std::ostream& operator<<(std::ostream& os, const glm::vec3& vec){
    return os << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
}




int width, height;
float ratio;
float scale;
int read_buffer = 0;
int write_buffer = 1;

const int N_ROWS = 7;
const int N_COLS = 7;
const int N_STACKS = 1;

const int N_TYPE0 = N_ROWS*(N_COLS-1)*N_STACKS;
const int N_TYPE1 = (N_ROWS-1)*N_COLS*N_STACKS;
const int N_TYPE2 = N_ROWS*N_COLS*(N_STACKS-1);
const int N_TYPE3 = (N_ROWS-1)*(N_COLS-1)*N_STACKS;
const int N_TYPE4 = (N_ROWS-1)*(N_COLS-1)*N_STACKS;
const int N_TYPE5 = N_ROWS*(N_COLS-1)*(N_STACKS-1);
const int N_TYPE6 = N_ROWS*(N_COLS-1)*(N_STACKS-1);
const int N_TYPE7 = (N_ROWS-1)*N_COLS*(N_STACKS-1);
const int N_TYPE8 = (N_ROWS-1)*N_COLS*(N_STACKS-1);
const int N_TYPE9 = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
const int N_TYPE10 = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
const int N_TYPE11 = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
const int N_TYPE12 = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);

const int N_MASSES = N_ROWS*N_COLS;
const int N_CONNECTIONS =
    N_TYPE0 +
    N_TYPE1 +
    N_TYPE2 +
    N_TYPE3 +
    N_TYPE4 +
    N_TYPE5 +
    N_TYPE6 +
    N_TYPE7 +
    N_TYPE8 +
    N_TYPE9 +
    N_TYPE10 +
    N_TYPE11 +
    N_TYPE12;

const int N_LENGTH_1 =
    N_TYPE0 +
    N_TYPE1 +
    N_TYPE2;

const int N_LENGTH_SQRT2 =
    N_TYPE3 +
    N_TYPE4 +
    N_TYPE5 + 
    N_TYPE6 +
    N_TYPE7 +
    N_TYPE8;

const int N_LENGTH_SQRT3 =
    N_TYPE9 +
    N_TYPE10 +
    N_TYPE11 + 
    N_TYPE12;

float masses[N_MASSES];
glm::vec2 positions[N_MASSES][2];
glm::vec2 velocities[N_MASSES][2];
glm::vec2 forces[N_MASSES];

int connected_masses[N_CONNECTIONS][2];
float spring_constants[N_CONNECTIONS];
float damper_constants[N_CONNECTIONS];
float spring_lengths[N_CONNECTIONS];

const float g = 9.82f;



GLFWwindow* window;


GLuint vertexArray = GL_FALSE;
GLuint vertexPositionBuffer = GL_FALSE;
GLuint vertexColorBuffer = GL_FALSE;

GLint MVP_loc = -1;

GLuint programID;

// Vertexdata
std::vector<glm::vec3> vertex_position_data;
// Colordata
std::vector<glm::vec3> vertex_color_data;


int main(void){
    //Test
    testParticle();
    testConnection();
    testMCS();


    initGLFW();
    initOpenGL();
    scale = (float) fmax(N_ROWS,N_COLS);
    ratio = width / (float) height;



    /* INIT SIMULATION */

    // Set start values to masses, positions velocities and forces
    for (int i = 0; i < N_MASSES; ++i){
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
    for (int i = 0; i < N_CONNECTIONS; ++i){
        spring_constants[i] = 500.0f;
        damper_constants[i] = 5.0f;
        spring_lengths[i] = 1.0f;
    }

    // Set connection lengths sqrt2
    for (int i = N_LENGTH_1; i < N_LENGTH_1 + N_LENGTH_SQRT2; ++i){
        spring_lengths[i] *= sqrt(2);
    }

    // Set connection lengths sqrt3
    for (int i = N_LENGTH_1 + N_LENGTH_SQRT2; i < N_CONNECTIONS; ++i){
        spring_lengths[i] *= sqrt(3);
    }

    // Calculate connections
    for (int i = 0; i < N_CONNECTIONS; ++i){
        connection2massIndices3D(i, connected_masses[i][0], connected_masses[i][1], N_ROWS, N_COLS, N_STACKS);
    }



    int simulations_per_frame = 40;
    float T = 1/(60.0f*simulations_per_frame);
    float current_time;




    while (!glfwWindowShouldClose(window)){
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        for (int i = 0; i < simulations_per_frame; ++i)
        {
            /* SIMULATION */
            for (int connection_index = 0; connection_index < N_CONNECTIONS; ++connection_index){
                //connection properties
                float k = spring_constants[connection_index];
                float b = damper_constants[connection_index];
                float l = spring_lengths[connection_index];

                //position
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
                float spring_elongation = glm::length(delta_p) - l;
                //if (connection_index == 0.0f)
                    //std::cout << spring_elongation*spring_elongation << std::endl;
                float elongation_sign = spring_elongation >= 0.0f ? 1.0f : -1.0f;
                float parallel_delta_v_magnitude = glm::dot(delta_v,delta_p_hat);

                //Spring force is now proportional to the squared spring elongation
                float signed_spring_force = -k*spring_elongation;//*spring_elongation*elongation_sign;

                glm::vec2 force_from_connection = ( signed_spring_force - b*parallel_delta_v_magnitude )*delta_p_hat;
                forces[mass_index1] += force_from_connection;
                forces[mass_index2] -= force_from_connection; 
            }

            // Moving one mass 
            //double x_mouse;
            //double y_mouse;

            //glfwGetCursorPos(window, &x_mouse, &y_mouse);
            //positions[0][write_buffer] = glm::vec2(float(x_mouse-0.5*width)*2*scale/height, -float(y_mouse-0.5*height)*2*scale/height);
            //velocities[0][write_buffer] = glm::vec2(0.0f, 0.0f);

            float scalex = scale*ratio;

            //Calculate acceleration, velocity and position
            // OBS!!!! NOT UPDATING MASS 1 HERE NOW
            for (int mass_index = 0; mass_index < N_MASSES; ++mass_index){
                glm::vec2 a = forces[mass_index]/masses[mass_index] - glm::vec2(0.f,1.f)*g;

                glm::vec2 v = velocities[mass_index][read_buffer] + a*T;
                glm::vec2 p = positions[mass_index][read_buffer] + v*T;

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

                //Store information in backbuffer
                velocities[mass_index][write_buffer] = v;
                positions[mass_index][write_buffer] = p;

                //reset force
                forces[mass_index] = glm::vec2(0.0f, 0.0f);
            }
            //Swap simulation buffers
            read_buffer = (read_buffer+1)%2;
            write_buffer = (write_buffer+1)%2;
        }

        /* DRAW */
        
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

}

int initOpenGL(){
    
    //Init gl points
    glEnable( GL_POINT_SMOOTH );
    //glEnable( GL_BLEND );
    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glPointSize( 15.0 );

    //Init gl lines
    //glEnable( GL_LINE_SMOOTH );
    



    // Initialize GLEW (Create OpenGL context)
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return false;
    }

    // Current OpenGL version    
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    for (int i = 0; i < N_MASSES; ++i){
        vertex_position_data.push_back(glm::vec3(positions[i][0].x, positions[i][0].y, 0.f));
        vertex_color_data.push_back(glm::vec3(1.f, 0.f, 0.f));
    }

    std::cout <<
        vertex_position_data[0] << std::endl <<
        vertex_position_data[1] << std::endl <<
        vertex_position_data[2] << std::endl;

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
    /*
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio * scale, ratio * scale, -1.f * scale, 1.f * scale, 1.f * scale, -1.f * scale);

    //Draw masses
    glBegin(GL_POINTS);
    glColor3f(1.f, 0.f, 0.f);
    for (int i = 0; i < N_MASSES; ++i){
        //glVertex3f(positions[i][read_buffer][0],
        //           positions[i][read_buffer][1], 0.f);
    }
    glEnd();

    //Draw connections
    glBegin(GL_LINES);
    glColor3f(0.f, 1.f, 0.f);
    for (int i = 0; i < N_CONNECTIONS; ++i){
        glm::vec2 delta_p = positions[connected_masses[i][0]][read_buffer]
                           -positions[connected_masses[i][1]][read_buffer];
        float r = glm::abs(glm::length(delta_p)-spring_lengths[i]);
        glColor3f(r,1-r,0.0f);
        glVertex3f(positions[connected_masses[i][0]][read_buffer][0],
                   positions[connected_masses[i][0]][read_buffer][1], 0.f);
        glVertex3f(positions[connected_masses[i][1]][read_buffer][0],
                   positions[connected_masses[i][1]][read_buffer][1], 0.f);
    }
    glEnd()*/



    for (int i = 0; i < N_MASSES; ++i){
        vertex_position_data[i] = glm::vec3(positions[i][0].x, positions[i][0].y, 1);
    }








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

    glm::mat4 MVP = glm::ortho(-10.f, 10.f, -10.f, 10.f);//glm::rotate(glm::mat4(1), speed * (float) glfwGetTime(), glm::vec3(0,0,1));
 
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
