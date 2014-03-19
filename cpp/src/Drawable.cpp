#include "Drawable.h"

OpenGL_drawable::OpenGL_drawable(const MCS* mcs){
    vertexPositionBuffer = GL_FALSE;
    vertexColorBuffer = GL_FALSE;
    vertexNormalBuffer = GL_FALSE;
    vertexUVBuffer = GL_FALSE;

    vertexArray = GL_FALSE;
    elementBuffer = GL_FALSE;

    MVP_loc = -1;
    MV_loc = -1;
    V_loc = -1;
    M_loc = -1;
    lightPos_loc = -1;
    lightColor_loc = -1;
    
    
    vertex_position_data_ = &mcs->vertices.positions[0];
    vertex_normal_data_ = &mcs->vertices.normals[0];
    vertex_color_data_ = &mcs->vertices.colors[0];
    vertex_UV_data_ = &mcs->vertices.UVs[0];
    element_data_ = &mcs->triangles.triangleIndices[0];
    
    n_elements_ = mcs->triangles.triangleIndices.size() * 3;
    n_verts_ = mcs->vertices.positions.size();
    
    setUpBuffers();

}

GLint OpenGL_drawable::MVP_loc = -1;
GLint OpenGL_drawable::MV_loc = -1;
GLint OpenGL_drawable::V_loc = -1;
GLint OpenGL_drawable::M_loc = -1;
GLint OpenGL_drawable::lightPos_loc = -1;
GLint OpenGL_drawable::lightColor_loc = -1;

void OpenGL_drawable::setUpBuffers(){
    // Generate the element buffer
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * n_elements_, element_data_, GL_STATIC_DRAW);
    
    //generate the VAO
    glGenVertexArrays(1, &vertexArray);
    
    // Create and compile the shader
    programID = LoadShaders( "../../data/shaders/simple.vert", "../../data/shaders/simple.frag" );
    
    // Bind the VAO
    glBindVertexArray(vertexArray);
    
    //generate VBO for vertex positions
    glGenBuffers(1, &vertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
    //upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * n_verts_, vertex_position_data_, GL_STATIC_DRAW);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * n_verts_, vertex_color_data_, GL_STATIC_DRAW);
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
    glGenBuffers(1, &vertexNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBuffer);
    //upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * n_verts_, vertex_normal_data_, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
                          2,                  // attribute 2. No particular reason for 2, but must match the layout in the shader.
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          reinterpret_cast<void*>(0) // array buffer offset
                          );
    
    //generate VBO for vertex UV
    glGenBuffers(1, &vertexUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexUVBuffer);
    //upload data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * n_verts_, vertex_UV_data_, GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
                          3,                  // attribute 3. No particular reason for 3, but must match the layout in the shader.
                          2,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          reinterpret_cast<void*>(0) // array buffer offset
                          );
    
    // Unbind the current VAO
    glBindVertexArray(0);
    
    // Shader IDs
    MVP_loc = glGetUniformLocation( programID, "MVP");
    MV_loc = glGetUniformLocation( programID, "MV");
    V_loc = glGetUniformLocation( programID, "V");
    M_loc = glGetUniformLocation( programID, "M");
    
    lightPos_loc = glGetUniformLocation( programID, "lightPos_worldSpace");
    lightColor_loc = glGetUniformLocation( programID, "lightColor");
    
    //Texture
    textureID = loadBMP_custom("../../data/textures/empty.bmp");
    texture_loc = glGetUniformLocation( programID, "textureSampler");
    
    int err = glGetError();
    if (err > 0){
        std::cout << "Error in initOpenGL(const OpenGL_drawable&, const MCS&). Error code: " << err << std::endl;
    }
}

OpenGL_drawable::~OpenGL_drawable(){
    deleteBuffers();
}

void OpenGL_drawable::deleteBuffers(){
    // Release memory
    if(vertexPositionBuffer)
        glDeleteBuffers(1, &vertexPositionBuffer);
    if(vertexNormalBuffer)
        glDeleteBuffers(1, &vertexNormalBuffer);
    if(vertexColorBuffer)
        glDeleteBuffers(1, &vertexColorBuffer);
    if(vertexUVBuffer)
        glDeleteBuffers(1, &vertexUVBuffer);
    if(elementBuffer)
        glDeleteBuffers(1, &elementBuffer);
    

    if(vertexArray)
        glDeleteVertexArrays(1, &vertexArray);
    int err = glGetError();
    if (err > 0){
        std::cout << "ERROR in OpenGL_drawable::~OpenGL_drawable(). Error code: " << err << std::endl;
    }
}

void OpenGL_drawable::print() const{
	std::cout << "vertexArray: " << vertexArray << std::endl;
	std::cout << "vertexPositionBuffer: " << vertexPositionBuffer << std::endl;
	std::cout << "vertexColorBuffer: " << vertexColorBuffer << std::endl;
	std::cout << "elementBuffer: " << elementBuffer << std::endl;
	std::cout << "MVP_loc: " << MVP_loc << std::endl;
	std::cout << "programID: " << programID << std::endl;
	//std::cout << "vertex_color_data.size(): " << vertex_color_data.size() << std::endl;
}

void OpenGL_drawable::updateBuffers(const MCS *mcs, MatrixHandler* matrices){
    
    vertex_position_data_ = &mcs->vertices.positions[0];
    vertex_normal_data_ = &mcs->vertices.normals[0];
    vertex_color_data_ = &mcs->vertices.colors[0];
    vertex_UV_data_ = &mcs->vertices.UVs[0];
    element_data_ = &mcs->triangles.triangleIndices[0];
    
    n_elements_ = mcs->triangles.triangleIndices.size() * 3;
    n_verts_ = mcs->vertices.positions.size();
    
    // Do the matrix stuff
    matrices->calculateMatrices();
    
    glm::vec3 lightPos = glm::vec3(30,30,30);
    glm::vec3 lightColor = glm::vec3(1,1,1);
    
    // Bind the VAO (Contains the vertex buffers)
    glBindVertexArray(vertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * n_verts_, vertex_position_data_, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * n_verts_, vertex_color_data_, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * n_verts_, vertex_normal_data_, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vertexUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * n_verts_, vertex_UV_data_, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * n_elements_, element_data_, GL_STATIC_DRAW);
    
    // Bind shader
    glUseProgram(programID);
    
    // Matrix data
    glUniformMatrix4fv(MVP_loc, 1, GL_FALSE, &matrices->MVP[0][0]);
    glUniformMatrix4fv(M_loc, 1, GL_FALSE, &matrices->M[0][0]);
    glUniformMatrix4fv(MV_loc, 1, GL_FALSE, &matrices->MV[0][0]);
    glUniformMatrix4fv(V_loc, 1, GL_FALSE, &matrices->V[0][0]);
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Set our "textureSampler" sampler to user Texture Unit 0
    glUniform1i(texture_loc, 0);
    
    // Light data
    glUniform3fv(lightPos_loc, 1, &lightPos[0]);
    glUniform3fv(lightColor_loc, 1, &lightColor[0]);
}

void OpenGL_drawable::draw(){
    // Bind shader
    glUseProgram(programID);
    
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    
    // Draw the triangles !
    glDrawElements(
                   GL_TRIANGLES,      // mode
                   n_elements_,    // count
                   GL_UNSIGNED_INT,   // type
                   (void*)0           // element array buffer offset
                   );
    
    
    // Unbind VAO
    glBindVertexArray(0);
    
    // Unbind shader
    glUseProgram(0);
}

