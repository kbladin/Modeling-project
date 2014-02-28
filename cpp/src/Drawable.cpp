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

void OpenGL_drawable::print() const{
	std::cout << "vertexArray: " << vertexArray << std::endl;
	std::cout << "vertexPositionBuffer: " << vertexPositionBuffer << std::endl;
	std::cout << "vertexColorBuffer: " << vertexColorBuffer << std::endl;
	std::cout << "elementBuffer: " << elementBuffer << std::endl;
	std::cout << "MVP_loc: " << MVP_loc << std::endl;
	std::cout << "programID: " << programID << std::endl;
	std::cout << "vertex_color_data.size(): " << vertex_color_data.size() << std::endl;
}


bool OpenGL_Drawer::add(MCS& mcs){
	OpenGL_drawable openGL_drawable;

	//BIND SOME OPENGL STUFF
	for (int i = 0; i < mcs.getNumberOfParticles(); ++i){
        openGL_drawable.vertex_color_data.push_back(glm::vec3(float(rand())/RAND_MAX, float(rand())/RAND_MAX, float(rand())/RAND_MAX));
    }

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

    vecMCS.push_back(&mcs);
    vecDrawable.push_back(openGL_drawable);

    std::cout << "\nDONE openGL_drawable:" << std::endl;
    openGL_drawable.print();

    return true;
}

void OpenGL_Drawer::draw(){
	for (int i = 0; i < vecMCS.size(); ++i){
		assert(vecMCS[i] != NULL);
		const MCS& mcs = *vecMCS[i];
		const OpenGL_drawable& openGL_drawable = vecDrawable[i];


		
	    // Do the matrix stuff
	    float speed = 0.0f;


	    glm::mat4 M = glm::mat4(1.0f);
	    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), speed * (float) glfwGetTime(), glm::vec3(0.0f,1.0f,0.0f));

	    glm::mat4 translate = glm::translate(glm::vec3(0.0f,0.0f,-20.0f));

	    glm::mat4 V = translate * rotate;
	    glm::mat4 P = glm::perspective(45.0f, ratio, 0.1f, 100.f);

	    glm::mat4 MVP = P*V*M;


	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	    
	}
}



