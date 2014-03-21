#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
// Include GLEW. Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "ElasticMaterials_lib.h"

#include "shader.h"
#include "texture.h"
#include "Drawable.h"
#include "user_input.h"

// Global functions
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void initGLFW();
bool initOpenGL();
void cleanUpGLFW();

MCS * createFloppyThing();
MCS * createRollingDice();
MCS * createStandingSnake();
MCS * createCloth();
MCS * createSoftCube();
MCS * createSpinner();
MCS * createJelly();
MCS * createWaffle();

// Global variables
GLFWwindow* window;
MCS * mcs = NULL;
Camera* cam = NULL;
//OpenGL_drawable openGL_drawable;
OpenGL_drawable* drawable_mcs;
std::vector<OpenGL_drawable*> drawable_planes; //Vet inte varför det måste vara pekare men funkar inte annars

MatrixHandler* matrices;

int width, height;
float ratio;

bool pause = false;
bool forward = false;
bool backward = false;

GLuint programID;
GLuint dry_material_programID;

GLuint textureID;
GLuint faces_textureID;
GLuint waffle_textureID;
GLuint cloth1_textureID;
GLuint cloth2_textureID;
GLuint cloth3_textureID;
GLuint cloth4_textureID;
GLuint cloth5_textureID;
GLuint slime_textureID;
GLuint floor2_textureID;
GLuint floor3_textureID;
GLuint die_textureID;
GLuint tiling_textureID;
GLuint induction_textureID;
GLuint wall1_textureID;
GLuint wall2_textureID;

Material* ground_material;
Material* object_material;


int main(void){
    initGLFW();
    initOpenGL();

    mcs = createFloppyThing();

    cam = new Camera(window, mcs);
    matrices = new MatrixHandler(cam);
    
    // Create and compile the shader
    programID = LoadShaders( "../../data/shaders/simple.vert", "../../data/shaders/simple.frag" );
    
    textureID = loadBMP_custom("../../data/textures/empty.bmp");
    faces_textureID = loadBMP_custom("../../data/textures/empty1.bmp");
    waffle_textureID = loadBMP_custom("../../data/textures/waffle.bmp");
    cloth1_textureID = loadBMP_custom("../../data/textures/cloth1.bmp");
    cloth2_textureID = loadBMP_custom("../../data/textures/cloth2.bmp");
    cloth3_textureID = loadBMP_custom("../../data/textures/cloth3.bmp");
    cloth4_textureID = loadBMP_custom("../../data/textures/cloth4.bmp");
    cloth5_textureID = loadBMP_custom("../../data/textures/cloth5.bmp");
    slime_textureID = loadBMP_custom("../../data/textures/slime2.bmp");
    floor2_textureID = loadBMP_custom("../../data/textures/floor2.bmp");
    floor3_textureID = loadBMP_custom("../../data/textures/floor3.bmp");
    die_textureID = loadBMP_custom("../../data/textures/die.bmp");
    induction_textureID = loadBMP_custom("../../data/textures/induction.bmp");
    tiling_textureID = loadBMP_custom("../../data/textures/tiling.bmp");
    wall1_textureID = loadBMP_custom("../../data/textures/wall1.bmp");
    wall2_textureID = loadBMP_custom("../../data/textures/wall2.bmp");

    
    // INIT SIMULATION 
    int simulations_per_frame = 12;
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
    
    ground_material = new Material;
    object_material = new Material;
    object_material->wetness = 0.1f;

    
    drawable_mcs = new OpenGL_drawable(mcs, *object_material, programID, textureID);
    for (int i=0; i<mcs->collisionPlanes.size(); ++i) {
        drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, textureID));
    }
    
    int frame = 0;
    
    std::cout << "🐙" << std::endl;
    
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
        
        drawable_mcs->updateRuntimeBuffers(mcs, matrices);
        
        
        /*
        for (int i=0; i<mcs->collisionPlanes.size(); ++i) {
            drawable_planes[i].draw();
        }
         */
        
        //drawable_plane->draw();
        for (int i=0; i<mcs->collisionPlanes.size(); ++i) {
            drawable_planes[i]->draw();
        }
        drawable_mcs->draw();

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
    delete drawable_mcs;
    for (int i = 0; i<drawable_planes.size(); ++i) {
        delete drawable_planes[i];
    }
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
        drawable_mcs->deleteBuffers();
        drawable_mcs->setUpBuffers(textureID);
        mcs = createFloppyThing();
        cam->setTarget(mcs);
        drawable_mcs->updateAllBuffers(mcs, *ground_material, matrices,textureID);
        for (int i = 0; i<drawable_planes.size(); ++i) {
            delete drawable_planes[i];
        }
        drawable_planes.resize(0);
        for (int i=0; i<mcs->collisionPlanes.size(); ++i) {
            drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, faces_textureID));
        }
        std::cout << "Done" << std::endl;
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS){
        std::cout << "Loading Rolling Dice..." << std::endl;
        delete mcs;
        drawable_mcs->deleteBuffers();
        drawable_mcs->setUpBuffers(die_textureID);
        mcs = createRollingDice();
        cam->setTarget(mcs);
        drawable_mcs->updateAllBuffers(mcs, *ground_material, matrices,die_textureID);
        for (int i = 0; i<drawable_planes.size(); ++i) {
            delete drawable_planes[i];
        }
        drawable_planes.resize(0);
        for (int i=0; i<mcs->collisionPlanes.size(); ++i) {
            drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, floor2_textureID));
        }
        std::cout << "Done" << std::endl;
    }

    if (key == GLFW_KEY_3 && action == GLFW_PRESS){
        std::cout << "Loading Standing Snake..." << std::endl;
        delete mcs;
        drawable_mcs->deleteBuffers();
        drawable_mcs->setUpBuffers(textureID);
        mcs = createStandingSnake();
        cam->setTarget(mcs);
        drawable_mcs->updateAllBuffers(mcs, *ground_material, matrices,textureID);
        for (int i = 0; i<drawable_planes.size(); ++i) {
            delete drawable_planes[i];
        }
        drawable_planes.resize(0);
        for (int i=0; i<mcs->collisionPlanes.size(); ++i) {
            drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, textureID));
        }
        std::cout << "Done" << std::endl;
    }

    if (key == GLFW_KEY_4 && action == GLFW_PRESS){
        std::cout << "Loading Cloth..." << std::endl;
        delete mcs;
        drawable_mcs->deleteBuffers();
        drawable_mcs->setUpBuffers(cloth2_textureID);
        mcs = createCloth();
        cam->setTarget(mcs);
        drawable_mcs->updateAllBuffers(mcs, *ground_material, matrices, cloth2_textureID);
        for (int i = 0; i<drawable_planes.size(); ++i) {
            delete drawable_planes[i];
        }
        drawable_planes.resize(0);
        for (int i=0; i<mcs->collisionPlanes.size(); ++i) {
            drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, textureID));
        }
        std::cout << "Done" << std::endl;
    }

    if (key == GLFW_KEY_5 && action == GLFW_PRESS){
        std::cout << "Loading SoftCube..." << std::endl;
        delete mcs;
        drawable_mcs->deleteBuffers();
        drawable_mcs->setUpBuffers(faces_textureID);
        mcs = createSoftCube();
        cam->setTarget(mcs);
        drawable_mcs->updateAllBuffers(mcs, *ground_material, matrices,faces_textureID);
        for (int i = 0; i<drawable_planes.size(); ++i) {
            delete drawable_planes[i];
        }
        drawable_planes.resize(0);
        for (int i=0; i<mcs->collisionPlanes.size(); ++i) {
            switch (i) {
                case 0: // Ground
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, wall2_textureID));
                    break;
                case 1: // Wall
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, cloth3_textureID));
                    break;
                default:
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, cloth3_textureID));
                    break;
            }
        }
        std::cout << "Done" << std::endl;
    }
    
    if (key == GLFW_KEY_6 && action == GLFW_PRESS){
        std::cout << "Loading Spinner..." << std::endl;
        delete mcs;
        drawable_mcs->deleteBuffers();
        drawable_mcs->setUpBuffers(cloth1_textureID);
        mcs = createSpinner();
        cam->setTarget(mcs);
        drawable_mcs->updateAllBuffers(mcs, *ground_material, matrices,cloth1_textureID);
        for (int i = 0; i<drawable_planes.size(); ++i) {
            delete drawable_planes[i];
        }
        drawable_planes.resize(0);
        for (int i=0; i<mcs->collisionPlanes.size(); ++i) {
            switch (i) {
                case 0: // Ground
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, floor3_textureID));
                    break;
                case 1: // Wall
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, cloth4_textureID));
                    break;
                default:
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, cloth4_textureID));
                    break;
            }
        }
        std::cout << "Done" << std::endl;
    }
    
    if (key == GLFW_KEY_7 && action == GLFW_PRESS){
        std::cout << "Loading Jelly..." << std::endl;
        delete mcs;
        drawable_mcs->deleteBuffers();
        drawable_mcs->setUpBuffers(slime_textureID);
        mcs = createJelly();
        cam->setTarget(mcs);
      Material slime_material;
      slime_material.wetness = 0.1;
      slime_material.shinyness = 64;
      slime_material.specularity = 0.5;
        drawable_mcs->updateAllBuffers(mcs, slime_material, matrices,slime_textureID);
        for (int i = 0; i<drawable_planes.size(); ++i) {
            delete drawable_planes[i];
        }
        drawable_planes.resize(0);
        for (int i=0; i<mcs->collisionPlanes.size(); ++i) {
            switch (i) {
                case 0: // Ground
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, floor2_textureID));
                    break;
                case 1: // Wall
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, floor2_textureID));
                    break;
                default:
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, floor2_textureID));
                    break;
            }
        }
        std::cout << "Done" << std::endl;
    }
    if (key == GLFW_KEY_8 && action == GLFW_PRESS){
        std::cout << "Loading Waffle... " << std::endl;
        delete mcs;
        drawable_mcs->deleteBuffers();
        drawable_mcs->setUpBuffers(waffle_textureID);
        mcs = createWaffle();
        cam->setTarget(mcs);
      Material waffle_material;
      waffle_material.wetness = 0.0;
      waffle_material.shinyness = 4;
      waffle_material.specularity = 0.2;
        drawable_mcs->updateAllBuffers(mcs, waffle_material, matrices,waffle_textureID);
      
      Material tiling_material;
      tiling_material.wetness = 0.001;
      tiling_material.shinyness = 128;
      tiling_material.specularity = 0.3;
      
      Material induction_material;
      induction_material.wetness = 0.0;
      induction_material.shinyness = 512;
      induction_material.specularity = 0.4;
        for (int i = 0; i<drawable_planes.size(); ++i) {
            delete drawable_planes[i];
        }
        drawable_planes.resize(0);
        for (int i=0; i<mcs->collisionPlanes.size(); ++i) {
            switch (i) {
                case 0: // Ground
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], induction_material, programID, induction_textureID));
                    break;
                case 1: // Wall
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], tiling_material, programID, tiling_textureID));
                    break;
                default:
                    drawable_planes.push_back(new OpenGL_drawable(&mcs->collisionPlanes[i], *ground_material, programID, floor2_textureID));
                    break;
            }
            
        }
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

void cleanUpGLFW(){
    // Terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

MCS * createFloppyThing(){
    MCS * tmp_mcs = new MCS(20,7,2);
    tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*9.82f;
    tmp_mcs->connections.setSpringConstant(5000.0f);
    tmp_mcs->addRotation(glm::vec3(0.0,1.0,1.0),-5.0f);
    tmp_mcs->setAvgPosition(glm::vec3(-10,5,-10));
    tmp_mcs->setAvgVelocity(glm::vec3(0,0,0));
    tmp_mcs->addCollisionPlane(glm::vec3(0,1,0),    //normal of the plane
                               -5.0f,      //positions the plane on normal
                               1.0f,      //elasticity
                               0.3f);      //friction
    return tmp_mcs;
}

MCS * createRollingDice(){
    MCS * tmp_mcs = new MCS(3,3,3);
    tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*80.0f;
    tmp_mcs->addRotation(glm::vec3(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX),15.0f);
    tmp_mcs->setAvgPosition(glm::vec3(0,5,0));
    tmp_mcs->connections.setSpringConstant(100000.0f);
    tmp_mcs->setAvgVelocity(glm::vec3(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX)*50.0f);
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
    MCS * tmp_mcs = new MCS(31,31,1);
    tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*9.82f;
    tmp_mcs->addRotation(glm::vec3(1.0,0.5,0.0),1.0f);
    tmp_mcs->connections.setSpringConstant(10000.0f);
    tmp_mcs->setAvgPosition(glm::vec3(0,0,0));
    tmp_mcs->setAvgVelocity(glm::vec3(0,0,0));
    
    Lock l(tmp_mcs->INTERVAL_LAST_ROW_SKIP15);
    tmp_mcs->lock_ = l;

    return tmp_mcs;
}

MCS * createSoftCube(){
    int s = 7;
    MCS * tmp_mcs = new MCS(s,s,s);
    tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*9.82f;
    tmp_mcs->connections.setSpringConstant(70.0f);
    tmp_mcs->connections.setDamperConstant(80.0f);
    tmp_mcs->addRotation(glm::vec3(0.0,1.0,1.0),-2.0f);
    tmp_mcs->setAvgPosition(glm::vec3(0,0,0));
    tmp_mcs->setAvgVelocity(glm::vec3(0,20,-40));
    float scene_scale = 200.0f;
    tmp_mcs->addCollisionPlane(glm::vec3(0,1,0),    //normal of the plane
                               -5.0f,      //positions the plane on normal
                               1.0f,      //elasticity
                               0.1f,      //friction
                               scene_scale);
    tmp_mcs->addCollisionPlane(glm::vec3(0,0,1),    //normal of the plane
                               -scene_scale/2,      //positions the plane on normal
                               -0.9f,      //elasticity
                               1.0f,      //friction
                               scene_scale);    //scale
    return tmp_mcs;
}

MCS * createSpinner(){
        int s = 3;
        MCS * tmp_mcs = new MCS(s,s,s);
        tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*9.82f;
        tmp_mcs->connections.setSpringConstant(5000.0f);
        tmp_mcs->connections.setDamperConstant(10.0f);
        tmp_mcs->addRotation(glm::vec3(0.0,1.0,0.3),-50.0f);
        tmp_mcs->setAvgPosition(glm::vec3(0,-3,0));
        tmp_mcs->setAvgVelocity(glm::vec3(0,0,0));
        float scene_scale = 50.0f;
        tmp_mcs->addCollisionPlane(glm::vec3(0,1,0),    //normal of the plane
                                   -5.0f,      //positions the plane on normal
                                   1.0f,      //elasticity
                                   0.1f,
                                   scene_scale);      //friction
        tmp_mcs->addCollisionPlane(glm::vec3(1,0,0),    //normal of the plane
                               -scene_scale/2,      //positions the plane on normal
                               1.0f,      //elasticity
                               0.3f,      //friction
                               scene_scale);    //scale
        tmp_mcs->addCollisionPlane(glm::vec3(0,0,1),    //normal of the plane
                               -scene_scale/2,      //positions the plane on normal
                               1.0f,      //elasticity
                               0.3f,      //friction
                               scene_scale);    //scale
        tmp_mcs->addCollisionPlane(glm::vec3(-1,0,0.001),    //normal of the plane
                               -scene_scale/2,      //positions the plane on normal
                               1.0f,      //elasticity
                               0.3f,      //friction
                               scene_scale);    //scale
    return tmp_mcs;
}

MCS * createJelly(){
    MCS * tmp_mcs = new MCS(10,10,10); //Minns inte hur den var
    tmp_mcs->connections.setDamperConstant(0.1);
    tmp_mcs->connections.setSpringConstant(2200);
    tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*9.82f;
    tmp_mcs->addRotation(glm::vec3(0.0,1.0,2.0),-7.0f);
    tmp_mcs->setAvgPosition(glm::vec3(0,5,0));
    tmp_mcs->setAvgVelocity(glm::vec3(1,2,0));
    float scene_scale = 250.0f;
    tmp_mcs->addCollisionPlane(glm::vec3(0,1,0),    //normal of the plane
                               -5.0f,      //positions the plane on normal
                               1.0f,      //elasticity
                               0.3f,      //friction
                               scene_scale);
    tmp_mcs->addCollisionPlane(glm::vec3(0,0,1),    //normal of the plane
                               -scene_scale/2,      //positions the plane on normal
                               1.0f,      //elasticity
                               0.3f,      //friction
                               scene_scale);    //scale
    return tmp_mcs;
}

MCS * createWaffle(){
    MCS * tmp_mcs = new MCS(10,8,2);
    tmp_mcs->externalAcceleration = glm::vec3(0,-1,0)*30.0f;
    tmp_mcs->connections.setSpringConstant(5000.0f);
    tmp_mcs->connections.setDamperConstant(70.0f);
    //tmp_mcs->addRotation(glm::vec3(0.0,1.0,1.0),-5.0f);
    tmp_mcs->setAvgPosition(glm::vec3(0,-0.6,0));
    tmp_mcs->setAvgVelocity(glm::vec3(0,0,0.2));
    
    float scene_scale = 50.0f;
    tmp_mcs->addCollisionPlane(glm::vec3(0,1,0),    //normal of the plane
                               -5.0f,      //positions the plane on normal
                               1.0f,      //elasticity
                               0.3f,      //friction
                               scene_scale);    //scale
    tmp_mcs->addCollisionPlane(glm::vec3(0,0,1),    //normal of the plane
                               -scene_scale/2,      //positions the plane on normal
                               1.0f,      //elasticity
                               0.3f,      //friction
                               scene_scale);    //scale
    return tmp_mcs;
}