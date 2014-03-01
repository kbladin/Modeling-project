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
#include "NumericalMethods.h"
#include "test.h"

#include "Drawable.h"
#include "MCS.h"

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void initGLFW();
bool initOpenGL();

void cleanUpGLFW();



static std::ostream& operator<<(std::ostream& os, const glm::vec3& vec){
    return os << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
}

int width, height;
float ratio;
float scale;

GLFWwindow* window;


// THIS IS USED ONLY FOR MODERN OPENGL


bool initOpenGL(OpenGL_drawable& openGL_drawable, const MCS& mcs);
void draw(const OpenGL_drawable& openGL_drawable, const MCS& mcs);




int main(void){
    int a = 0;

    initGLFW();
    initOpenGL();

    scale = 11;// (float) fmax(N_ROWS,N_COLS);
    
    
    MCS mcs = MCS(20,7,2);
    mcs.externalAcceleration = glm::vec3(0,-1,0)*9.82f;
    mcs.addRotation(glm::vec3(0.0,1.0,1.0),-5.0f);
    mcs.setAvgPosition(glm::vec3(-5,0,-10));
    mcs.setAvgVelocity(glm::vec3(0,0,0));
    mcs.addCollisionPlane(glm::vec3(0,1,0),    //normal of the plane
                                   -10.0f,      //positions the plane on normal
                                    1.0f,      //elasticity
                                    0.3f);      //friction


    
    

    // INIT SIMULATION 
    int simulations_per_frame = 5;
    float dt = 1.0f/(60.0f*simulations_per_frame);

    std::vector<float> w;
    w.push_back(1.0f);
    w.push_back(3.0f);
    w.push_back(3.0f);
    w.push_back(1.0f);

    RungeKutta rk4(w);
    EulerExplicit ee;



    float current_time = glfwGetTime();;
    int FPS = 0;


    //OpenGL_Drawer od;
    //od.add(mcs);
    


    OpenGL_drawable openGL_drawable;
    initOpenGL(openGL_drawable, mcs);
    
    int b=0, frame = 0;
    while (!glfwWindowShouldClose(window)){

        for (int i = 0; i < simulations_per_frame; ++i){   
            // Moving one mass 
            //double x_mouse, y_mouse;
            //glfwGetCursorPos(window, &x_mouse, &y_mouse);
            //glm::vec2 pos2d = glm::vec2(float(x_mouse-0.5*width)*2*scale/height, -float(y_mouse-0.5*height)*2*scale/height);
            //mcs.setAvgPosition(glm::vec3(pos2d[0],pos2d[1],-50));
            //mcs.particles.positions[0] = glm::vec3(pos2d[0],pos2d[1],-50);
            //mcs.particles.velocities[0] = glm::vec3(0);

            rk4.update(mcs,dt);
        }
        
        // DRAW
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);
        //od.ratio = width / (float) height;
        //od.draw();
        

        //draw(od.vecDrawable[0], od_mcs);
        //draw(od.vecDrawable[0], *od.vecMCS[0]);
        
        draw(openGL_drawable, mcs);

        //Swap draw buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Print FPS
        ++FPS;
        ++frame;
        if((glfwGetTime() - current_time) > 1){
            std::string title = "Elastic materials, ";
            std::ostringstream ss;
            ss << FPS;
            std::string s(ss.str());
            title.append(s);
            title.append(" FPS");
            glfwSetWindowTitle (window,  title.c_str());
            FPS = 0;
            current_time = glfwGetTime();
        }
    }
    cleanUpGLFW();
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
    window = glfwCreateWindow(640, 480, "Elastic materials", NULL, NULL);
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
    /*
    glEnable( GL_POINT_SMOOTH );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glPointSize( 5.0 );

    //Init gl lines
    glEnable( GL_LINE_SMOOTH );
    */


    // INITIALISATION OF MODERN OPENGL

    // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return false;
    }
    int err = glGetError(); 
    if (err > 0){
        //OBS!!!!
        //glewInit() causes OpenGL error 1280 (GL_INVALID_ENUM)
        //this is probably safe to ignore if no other strange 
        //things happen
        std::cout << "Error in initOpenGL(). Error code = " << err << std::endl;
    }

    glEnable(GL_DEPTH_TEST);
    // Current OpenGL version    
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;


    return true;
}


bool initOpenGL(OpenGL_drawable& openGL_drawable, const MCS& mcs){
    for (int i = 0; i < mcs.getNumberOfParticles(); ++i){
        openGL_drawable.vertex_color_data.push_back(glm::vec3(float(rand())/RAND_MAX, float(rand())/RAND_MAX, float(rand())/RAND_MAX));
    }

    //glEnable(GL_DEPTH_TEST);

    // Generate the element buffer
    glGenBuffers(1, &openGL_drawable.elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGL_drawable.elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 3 * mcs.triangles.triangleIndices.size(), &mcs.triangles.triangleIndices[0], GL_STATIC_DRAW);

    //generate the VAO
    glGenVertexArrays(1, &openGL_drawable.vertexArray);

    // Create and compile the shader
    openGL_drawable.programID = LoadShaders( "data/shaders/simple.vert", "data/shaders/simple.frag" );

    // Bind the VAO (will contain one vertex position buffer and one vertex color buffer)
    glBindVertexArray(openGL_drawable.vertexArray);
 
    //generate VBO for vertex positions
    glGenBuffers(1, &openGL_drawable.vertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, openGL_drawable.vertexPositionBuffer);
    //upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mcs.particles.positions.size(), &mcs.particles.positions[0], GL_STATIC_DRAW);
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
    glGenBuffers(1, &openGL_drawable.vertexColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, openGL_drawable.vertexColorBuffer);
    //upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * openGL_drawable.vertex_color_data.size(), &openGL_drawable.vertex_color_data[0], GL_STATIC_DRAW);
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
    openGL_drawable.MVP_loc = glGetUniformLocation( openGL_drawable.programID, "MVP");

    //UNBIND SHADER HERE
    // ------

    return true;

}

void draw(const OpenGL_drawable& openGL_drawable, const MCS& mcs){

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

    ratio = width / (float) height;

    // Do the matrix stuff
    float speed = 0.0f;
    glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), speed * (float) glfwGetTime(), glm::vec3(0.0f,1.0f,0.0f));
    glm::mat4 translate = glm::translate(glm::vec3(0.0f,0.0f,-20.0f));
    glm::mat4 V = translate * rotate;
    glm::mat4 P = glm::perspective(45.0f, ratio, 0.1f, 100.f);
    glm::mat4 MVP = P*V*M;

    // Bind the VAO (will contain one vertex position buffer and one vertex color buffer)
    glBindVertexArray(openGL_drawable.vertexArray);
 
    // Bind position buffer
    glBindBuffer(GL_ARRAY_BUFFER, openGL_drawable.vertexPositionBuffer);
    //upload data to GPU
    // THIS IS NOR SUPER (SENDING DATA TO GPU EVERY FRAME)
    // (Not needed for cube but done anyway since this is how it will be done)
    glBufferData(GL_ARRAY_BUFFER, sizeof(int) * 3 * mcs.triangles.triangleIndices.size(), &mcs.particles.positions[0], GL_STATIC_DRAW);
 
    // Bind color buffer
    glBindBuffer(GL_ARRAY_BUFFER, openGL_drawable.vertexColorBuffer);
    //upload data to GPU
    // THIS IS NOR SUPER (SENDING DATA TO GPU EVERY FRAME)
    glBufferData(GL_ARRAY_BUFFER, sizeof(int) * 3 * openGL_drawable.vertex_color_data.size(), &openGL_drawable.vertex_color_data[0], GL_STATIC_DRAW);
    
    //BIND SHADER HERE
    glUseProgram(openGL_drawable.programID);
 
    glUniformMatrix4fv(openGL_drawable.MVP_loc, 1, GL_FALSE, &MVP[0][0]);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGL_drawable.elementBuffer);

    // Draw the triangles !
    glDrawElements(
     GL_TRIANGLES,      // mode
     mcs.triangles.triangleIndices.size()*3,    // count
     GL_UNSIGNED_INT,   // type
     (void*)0           // element array buffer offset
    );


 
    //unbind
    glBindVertexArray(0);
    
    //UNBIND SHADER HERE
    glUseProgram(0);

    int err = glGetError();
    if (err > 0){
        std::cout << "Error in draw(const OpenGL_drawable&, const MCS&). Error code: " << err << std::endl;
    }
}

void cleanUpGLFW()
{
    // Terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

