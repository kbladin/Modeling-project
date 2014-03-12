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
#include "conversions.h"
#include "NumericalMethods.h"
#include "debugtools.h"

#include "Drawable.h"
#include "MCS.h"
#include "user_input.h"
#include "MatrixHandler.h"

// Global functions
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void initGLFW();
bool initOpenGL();
void cleanUpGLFW();

bool initOpenGL(OpenGL_drawable& openGL_drawable, const MCS& mcs);
bool draw(const OpenGL_drawable& openGL_drawable, const MCS& mcs);

MCS * createFloppyThing();
MCS * createRollingDice();
MCS * createStandingSnake();
MCS * createCloth();
MCS * createSoftCube();


// Global variables
GLFWwindow* window;
MCS * mcs = NULL;
Camera* cam = NULL;
OpenGL_drawable openGL_drawable;
MatrixHandler* matrices;

int width, height;
float ratio;

bool pause = false;
bool forward = false;
bool backward = false;

int main(void){
    initGLFW();
    initOpenGL();

    mcs = createFloppyThing();

    cam = new Camera(window, mcs);
    matrices = new MatrixHandler(cam);
    
    // INIT SIMULATION 
    int simulations_per_frame = 10;
    float dt = 1.0f/(60.0f*simulations_per_frame);

    std::vector<float> w;
    w.push_back(1.0f);
    w.push_back(3.0f);
    w.push_back(3.0f);
    w.push_back(1.0f);

    RungeKutta rk4(w);
    EulerExplicit ee;
    NumericalMethod * nm = &ee; //Make use of polymorphism

    float current_time = glfwGetTime();;
    int FPS = 0;

    //OpenGL_Drawer //od;
    ////od.add(mcs);
    
    initOpenGL(openGL_drawable, *mcs);
    
    int frame = 0;
    
    while (!glfwWindowShouldClose(window)){

        // Moving one mass 
        //double x_mouse, y_mouse;
        //glfwGetCursorPos(window, &x_mouse, &y_mouse);
        //glm::vec2 pos2d = glm::vec2(float(x_mouse-0.5*width)*2*scale/height, -float(y_mouse-0.5*height)*2*scale/height);
        //mcs->setAvgPosition(glm::vec3(pos2d[0],pos2d[1],-50));
        //mcs->vertices.positions[0] = glm::vec3(pos2d[0],pos2d[1],-50);
        //mcs->particles.velocities[0] = glm::vec3(0);
        if(!pause){
            for (int i = 0; i < simulations_per_frame; ++i){   
                nm->update(*mcs,dt);
            }
        }
        else{
            for (int i = 0; i < simulations_per_frame; ++i){
                if(forward) nm->update(*mcs,dt/10.0f);
                if(backward) nm->update(*mcs,-dt/10.0f);
            }
        }
        mcs->updateVertexPositions();
        mcs->updateNormals();

        // DRAW
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwGetFramebufferSize(window, &width, &height);
        //od.ratio = width / (float) height;
        //if(!od.draw()) break;
        

        //if(!draw(od.vecDrawable[0], mcs)) break;
        //if(!draw(od.vecDrawable[0], *od.vecMCS[0])) break;
        if(!draw(openGL_drawable, *mcs)) break;

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
    delete mcs;
    delete cam;
    cleanUpGLFW();
}

static void error_callback(int error, const char* description){
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){

    //EXIT
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    //PLAYBACK
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        pause = !pause;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        forward = true;
    if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
        forward = false;
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        backward = true;
    if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
        backward = false;


    //MODIFICATION
    if (key == GLFW_KEY_M && action == GLFW_PRESS){
        float m = readFloat("Enter particle mass: ");
        mcs->particles.setMasses(m, Particles::ALL);
    }

    if (key == GLFW_KEY_L && action == GLFW_PRESS){
        float l = readFloat("Enter connection length: ");
        mcs->connections.setLengths(l);
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS){
        float s = readFloat("Enter spring constant: ");
        mcs->connections.setSpringConstant(s);
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS){
        float d = readFloat("Enter damper constant: ");
        mcs->connections.setDamperConstant(d);
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS){
        mcs->freeze();
    }

    //INITS
    if (key == GLFW_KEY_1 && action == GLFW_PRESS){
        std::cout << "Loading Floppy Thing... " << std::endl;
        delete mcs;
        openGL_drawable.deleteBuffers();
        mcs = createFloppyThing();
        cam->setTarget(mcs);
        initOpenGL(openGL_drawable, *mcs);
        std::cout << "Done" << std::endl;
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS){
        std::cout << "Loading Rolling Dice..." << std::endl;
        delete mcs;
        openGL_drawable.deleteBuffers();
        mcs = createRollingDice();
        cam->setTarget(mcs);
        initOpenGL(openGL_drawable, *mcs);
        std::cout << "Done" << std::endl;
    }

    if (key == GLFW_KEY_3 && action == GLFW_PRESS){
        std::cout << "Loading Standing Snake..." << std::endl;
        delete mcs;
        openGL_drawable.deleteBuffers();
        mcs = createStandingSnake();
        cam->setTarget(mcs);
        initOpenGL(openGL_drawable, *mcs);
        std::cout << "Done" << std::endl;
    }

    if (key == GLFW_KEY_4 && action == GLFW_PRESS){
        std::cout << "Loading Cloth..." << std::endl;
        delete mcs;
        openGL_drawable.deleteBuffers();
        mcs = createCloth();
        cam->setTarget(mcs);
        initOpenGL(openGL_drawable, *mcs);
        std::cout << "Done" << std::endl;
    }

    if (key == GLFW_KEY_5 && action == GLFW_PRESS){
        std::cout << "Loading SoftCube..." << std::endl;
        delete mcs;
        openGL_drawable.deleteBuffers();
        mcs = createSoftCube();
        cam->setTarget(mcs);
        initOpenGL(openGL_drawable, *mcs);
        std::cout << "Done" << std::endl;
    }

}

void initGLFW(){
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

    glClearColor(0.5f,0.5f,0.5f,0.0f);
    
    glEnable(GL_CULL_FACE);

    return true;
}


bool initOpenGL(OpenGL_drawable& openGL_drawable, const MCS& mcs){

    // Generate the element buffer
    glGenBuffers(1, &openGL_drawable.elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGL_drawable.elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 3 * mcs.triangles.triangleIndices.size(), &mcs.triangles.triangleIndices[0], GL_STATIC_DRAW);

    //generate the VAO
    glGenVertexArrays(1, &openGL_drawable.vertexArray);

    // Create and compile the shader
    openGL_drawable.programID = LoadShaders( "data/shaders/simple.vert", "data/shaders/simple.frag" );

    // Bind the VAO
    glBindVertexArray(openGL_drawable.vertexArray);
 
    //generate VBO for vertex positions
    glGenBuffers(1, &openGL_drawable.vertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, openGL_drawable.vertexPositionBuffer);
    //upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mcs.vertices.positions.size(), &mcs.vertices.positions[0], GL_STATIC_DRAW);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mcs.vertices.colors.size(), &mcs.vertices.colors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        reinterpret_cast<void*>(0) // array buffer offset
    );

    //generate VBO for vertex normals
    glGenBuffers(1, &openGL_drawable.vertexNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, openGL_drawable.vertexNormalBuffer);
    //upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mcs.vertices.normals.size(), &mcs.vertices.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,                  // attribute 2. No particular reason for 2, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        reinterpret_cast<void*>(0) // array buffer offset
    );
    
    // Unbind the current VAO
    glBindVertexArray(0);
 
    // Shader IDs
    openGL_drawable.MVP_loc = glGetUniformLocation( openGL_drawable.programID, "MVP");
    openGL_drawable.MV_loc = glGetUniformLocation( openGL_drawable.programID, "MV");
    openGL_drawable.V_loc = glGetUniformLocation( openGL_drawable.programID, "V");
    openGL_drawable.M_loc = glGetUniformLocation( openGL_drawable.programID, "M");

    openGL_drawable.lightPos_loc = glGetUniformLocation( openGL_drawable.programID, "lightPos_worldSpace");
    openGL_drawable.lightColor_loc = glGetUniformLocation( openGL_drawable.programID, "lightColor");

    int err = glGetError();
    if (err > 0){
        std::cout << "Error in initOpenGL(const OpenGL_drawable&, const MCS&). Error code: " << err << std::endl;
        return false;
    }

    return true;
}

bool draw(const OpenGL_drawable& openGL_drawable, const MCS& mcs){

    ratio = width / (float) height;

    // Do the matrix stuff
    matrices->calculateMatrices();

    glm::vec3 lightPos = glm::vec3(30,30,0);
    glm::vec3 lightColor = glm::vec3(1,1,1);

    // Bind the VAO (Contains the vertex buffers)
    glBindVertexArray(openGL_drawable.vertexArray);
 
    // Bind buffers and upload data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, openGL_drawable.vertexPositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mcs.vertices.positions.size(), &mcs.vertices.positions[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, openGL_drawable.vertexColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mcs.vertices.colors.size(), &mcs.vertices.colors[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, openGL_drawable.vertexNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mcs.vertices.normals.size(), &mcs.vertices.normals[0], GL_STATIC_DRAW);

    // Bind shader
    glUseProgram(openGL_drawable.programID); 

    // Matrix data
    glUniformMatrix4fv(openGL_drawable.MVP_loc, 1, GL_FALSE, &matrices->MVP[0][0]);
    glUniformMatrix4fv(openGL_drawable.M_loc, 1, GL_FALSE, &matrices->M[0][0]);
    glUniformMatrix4fv(openGL_drawable.MV_loc, 1, GL_FALSE, &matrices->MV[0][0]);
    glUniformMatrix4fv(openGL_drawable.V_loc, 1, GL_FALSE, &matrices->V[0][0]);

    // Light data
    glUniform3fv(openGL_drawable.lightPos_loc, 1, &lightPos[0]);
    glUniform3fv(openGL_drawable.lightColor_loc, 1, &lightColor[0]);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGL_drawable.elementBuffer);

    // Draw the triangles !
    glDrawElements(
     GL_TRIANGLES,      // mode
     mcs.triangles.triangleIndices.size()*3,    // count
     GL_UNSIGNED_INT,   // type
     (void*)0           // element array buffer offset
    );

    // Unbind VAO
    glBindVertexArray(0);
    
    // Unbind shader
    glUseProgram(0);

    int err = glGetError();
    if (err > 0){
        std::cout << "Error in draw(const OpenGL_drawable&, const MCS&). Error code: " << err << std::endl;
        return false;
    }
    return true;
}


void cleanUpGLFW(){
    // Terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

/*
void setMCS(){
  
    std::cout << "Loading Cloth..." << std::endl;
    openGL_drawable.deleteBuffers();
    mcs = createCloth();
    initOpenGL(openGL_drawable, *mcs);
    std::cout << "Done" << std::endl;

}
*/

MCS * createFloppyThing(){
    MCS * tmp_mcs = new MCS(3,12,20); //Minns inte hur den var
    tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*9.82f;
    tmp_mcs->addRotation(glm::vec3(0.0,1.0,1.0),-5.0f);
    tmp_mcs->setAvgPosition(glm::vec3(0,5,0));
    tmp_mcs->setAvgVelocity(glm::vec3(0,0,0));
    tmp_mcs->addCollisionPlane(glm::vec3(0,1,0),    //normal of the plane
                                   -5.0f,      //positions the plane on normal
                                    1.0f,      //elasticity
                                    0.3f);      //friction
    return tmp_mcs;
}

MCS * createRollingDice(){
    MCS * tmp_mcs = new MCS(3,3,3);
    tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*9.82f;
    tmp_mcs->addRotation(glm::vec3(0.0,1.0,1.0),25.0f);
    tmp_mcs->setAvgPosition(glm::vec3(0,0,0));
    tmp_mcs->connections.setSpringConstant(20000.0f);
    tmp_mcs->setAvgVelocity(glm::vec3(8,5,0));
    tmp_mcs->addCollisionPlane(glm::vec3(0,1,0),    //normal of the plane
                                   -5.0f,      //positions the plane on normal
                                    1.0f,      //elasticity
                                    0.3f);      //friction
    return tmp_mcs;
}

MCS * createStandingSnake(){
    int h = 400;
    MCS * tmp_mcs = new MCS(h,2,2);
    tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*9.82f;
    tmp_mcs->addRotation(glm::vec3(0.0,1.0,0.0),0.0f);
    tmp_mcs->connections.setSpringConstant(100000.0f);
    tmp_mcs->setAvgPosition(glm::vec3(0,h/2-6,-15));
    tmp_mcs->setAvgVelocity(glm::vec3(0,0,0));
    tmp_mcs->addCollisionPlane(glm::vec3(0,1,0),    //normal of the plane
                                   -5.0f,      //positions the plane on normal
                                    1.0f,      //elasticity
                                    0.3f);      //friction
    return tmp_mcs;
}

MCS * createCloth(){
    MCS * tmp_mcs = new MCS(30,30,1);
    //tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*9.82f;
    tmp_mcs->connections.setLengths(0.8f);
    tmp_mcs->addRotation(glm::vec3(0.0,1.0,1.0),0.5f);
    tmp_mcs->connections.setSpringConstant(100000.0f);
    tmp_mcs->setAvgPosition(glm::vec3(0,0,0));
    tmp_mcs->setAvgVelocity(glm::vec3(0,0,0));

    return tmp_mcs;
}

MCS * createSoftCube(){
    int s = 5;
    MCS * tmp_mcs = new MCS(s,s,s);
    tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*9.82f;
    tmp_mcs->connections.setSpringConstant(100.0f);
    tmp_mcs->connections.setDamperConstant(70.0f);
    tmp_mcs->addRotation(glm::vec3(0.0,1.0,1.0),-2.0f);
    tmp_mcs->setAvgPosition(glm::vec3(0,0,0));
    tmp_mcs->setAvgVelocity(glm::vec3(20,20,0));
    tmp_mcs->addCollisionPlane(glm::vec3(0,1,0),    //normal of the plane
                                   -5.0f,      //positions the plane on normal
                                    1.0f,      //elasticity
                                    0.3f);      //friction
    
    return tmp_mcs;
}
