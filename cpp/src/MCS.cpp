#include "MCS.h"

//Constructor
MCS::MCS(const int n_rows, const int n_cols, const int n_stacks):   
    N_ROWS(n_rows),N_COLS(n_cols),N_STACKS(n_stacks)
{
	initParticles();
    initConnections();

    initVertices();
    initTriangles();

    externalAcceleration = glm::vec3(0,0,0);
    externalForce = glm::vec3(0,0,0);
}

void MCS::initParticles(){

    int numberOfParticles = N_COLS*N_ROWS*N_STACKS;

    particles.positions = std::vector<glm::vec3>(numberOfParticles);
    particles.velocities = std::vector<glm::vec3>(numberOfParticles);
    particles.accelerations = std::vector<glm::vec3>(numberOfParticles);
    particles.forces = std::vector<glm::vec3>(numberOfParticles);
    particles.masses = std::vector<float>(numberOfParticles);
    //particles.normals = std::vector<glm::vec3>(numberOfParticles);

    for (int i = 0; i < numberOfParticles; ++i){
        float x = i%N_COLS;
        float y = (i/N_COLS)%N_ROWS;
        float z = i/(N_COLS*N_ROWS);

        particles.positions[i] = glm::vec3(x,y,z);
        particles.velocities[i]  = glm::vec3(0.f,0.f,0.f);
        particles.accelerations[i] = glm::vec3(0.f,0.f,0.f);
        particles.forces[i] = glm::vec3(0.f,0.f,0.f);
        particles.masses[i] = 1.0f;
        //particles.normals[i] = glm::vec3(1.0f,1.0f,1.0f);
    } 
}

void MCS::initConnections(){
    connections.nType[0] = N_ROWS*(N_COLS-1)*N_STACKS;
    connections.nType[1] = (N_ROWS-1)*N_COLS*N_STACKS;
    connections.nType[2] = N_ROWS*N_COLS*(N_STACKS-1);
    connections.nType[3] = (N_ROWS-1)*(N_COLS-1)*N_STACKS;
    connections.nType[4] = (N_ROWS-1)*(N_COLS-1)*N_STACKS;
    connections.nType[5] = N_ROWS*(N_COLS-1)*(N_STACKS-1);
    connections.nType[6] = N_ROWS*(N_COLS-1)*(N_STACKS-1);
    connections.nType[7] = (N_ROWS-1)*N_COLS*(N_STACKS-1);
    connections.nType[8] = (N_ROWS-1)*N_COLS*(N_STACKS-1);
    connections.nType[9] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    connections.nType[10] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    connections.nType[11] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    connections.nType[12] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);

    connections.startType[0] = 0;
    int numberOfConnections = connections.nType[0];
    for (int i = 1; i < 13; ++i){
        connections.startType[i] = connections.startType[i-1] + connections.nType[i-1];
        numberOfConnections += connections.nType[i];
    }


    connections.lengths = std::vector<float>(numberOfConnections);
    connections.springConstants = std::vector<float>(numberOfConnections);
    connections.damperConstants = std::vector<float>(numberOfConnections);
    connections.particle1 = std::vector<int>(numberOfConnections);
    connections.particle2 = std::vector<int>(numberOfConnections);

    // Calculate connections
    int p_index1;
    int p_index2;
    for (int i = 0; i < numberOfConnections; ++i){
        connection2massIndices3D(i, p_index1, p_index2, N_ROWS, N_COLS, N_STACKS);
        
        connections.lengths[i] = 1.0f;
        connections.springConstants[i] = 2000.0f;
        connections.damperConstants[i] = 20.0f;
        connections.particle1[i] = p_index1;
        connections.particle2[i] = p_index2;
    }
    //Set length of 2D-diagonal springs to sqrt(2)
    for (int i = connections.startType[3]; i < connections.startType[9]; ++i){
        connections.lengths[i] *= sqrt(2.0f);
    }

    //Set length of 3D-diagonal springs to sqrt(3)
    for (int i = connections.startType[9]; i < numberOfConnections; ++i){
        connections.lengths[i] *= sqrt(3.0f);
    }
}

void MCS::addCollisionPlane(glm::vec3 normal, float position, float elasticity, float friction){
    CollisionPlane cp; 
    cp.normal = glm::normalize(normal);
    cp.position = position;
    cp.elasticity = elasticity;
    cp.friction = friction;
    collisionPlanes.push_back(cp);
}

void MCS::initTriangles(){
    if(vertices.positions.size() == 0){
        std::cout << "ERROR: Vertices not initialized before triangles" << std::endl;
        assert(false);
    }
    const int n_plane1 = 2*((N_ROWS-1)*(N_COLS-1));
    const int n_plane2 = n_plane1;
    const int n_plane3 = 2*((N_ROWS-1)*(N_STACKS-1));
    const int n_plane4 = n_plane3;
    const int n_plane5 = 2*((N_COLS-1)*(N_STACKS-1));
    const int n_plane6 = n_plane5;

    int n_triangles =
        n_plane1 + n_plane2 + 
        n_plane3 + n_plane4 + 
        n_plane5 + n_plane6;

    triangles.triangleIndices = std::vector<IndexedTriangle>(n_triangles);
    triangles.normals = std::vector<glm::vec3>(n_triangles);

    for(int ti = 0; ti < n_triangles; ++ti){
        triangle2vertexIndices(
            ti,
            triangles.triangleIndices[ti].idx1,
            triangles.triangleIndices[ti].idx2,
            triangles.triangleIndices[ti].idx3);
        triangles.normals[ti] = glm::vec3(0.0f,0.0f,0.0f);
        std::cout << "ti = " << ti << std::endl;
        std::cout << "triangles.triangleIndices[ti].idx1 = " << triangles.triangleIndices[ti].idx1 << std::endl;
        std::cout << "triangles.triangleIndices[ti].idx2 = " << triangles.triangleIndices[ti].idx2 << std::endl;
        std::cout << "triangles.triangleIndices[ti].idx3 = " << triangles.triangleIndices[ti].idx3 << std::endl;
    }
}

void MCS::initVertices(){
    const int n_plane1 = (N_ROWS == 1 || N_COLS == 1) ? 0 : N_ROWS * N_COLS;
    const int n_plane2 = n_plane1;
    const int n_plane3 = (N_ROWS == 1 || N_STACKS == 1) ? 0 : N_ROWS * N_STACKS;
    const int n_plane4 = n_plane3;
    const int n_plane5 = (N_COLS == 1 || N_STACKS == 1) ? 0 : N_COLS * N_STACKS;
    const int n_plane6 = n_plane5;

    int n_vertices =
        n_plane1 + n_plane2 + 
        n_plane3 + n_plane4 + 
        n_plane5 + n_plane6;

    vertices.positions = std::vector<glm::vec3>(n_vertices);
    vertices.normals = std::vector<glm::vec3>(n_vertices);
    vertices.colors = std::vector<glm::vec3>(n_vertices);
    vertices.UVs = std::vector<glm::vec2>(n_vertices);
    vertices.particleIndices = std::vector<int>(n_vertices);

    for(int i = 0; i < n_vertices; ++i){
        vertex2particleIndex(i, vertices.particleIndices[i]);
        vertices.positions[i] = particles.positions[vertices.particleIndices[i]];
        vertices.normals[i] = glm::vec3(0,0,0);
        vertices.colors[i] = glm::vec3(float(rand())/RAND_MAX, float(rand())/RAND_MAX, float(rand())/RAND_MAX);
    }
}

void MCS::calcConnectionForcesOnParticles(const std::vector<glm::vec3>& delta_v_offset, const std::vector<glm::vec3>& delta_p_offset){
    glm::vec3 delta_p;
    glm::vec3 delta_v;
    glm::vec3 delta_p_hat;
    glm::vec3 force;

    float k;
    float l;
    float b;
    float spring_elongation;

    // Calculate the forces of the springs
    for (int i = 0; i < getNumberOfConnections(); ++i){
        k = connections.springConstants[i];
        l = connections.lengths[i];
        b = connections.damperConstants[i];

        delta_p = particles.positions[connections.particle1[i]] - particles.positions[connections.particle2[i]]; 
        delta_p += delta_p_offset[i];
        delta_p_hat = glm::normalize(delta_p);

        delta_v = particles.velocities[connections.particle1[i]] - particles.velocities[connections.particle2[i]]; 
        delta_v += delta_v_offset[i];
        
        spring_elongation = glm::length(delta_p) - l;
        //float sign = spring_elongation >= 0.0f ? 1.0f : -1.0f;

        force = (-k*spring_elongation - b*glm::dot(delta_v,delta_p_hat))*delta_p_hat;
        particles.forces[connections.particle1[i]] += force;
        particles.forces[connections.particle2[i]] -= force;
    }
}

void MCS::calcConnectionForcesOnParticles(){
    glm::vec3 delta_p;
    glm::vec3 delta_v;
    glm::vec3 delta_p_hat;
    glm::vec3 force;

    float k;
    float l;
    float b;
    float spring_elongation;

    // Calculate the forces of the springs
    for (int i = 0; i < getNumberOfConnections(); ++i){
        k = connections.springConstants[i];
        l = connections.lengths[i];
        b = connections.damperConstants[i];

        delta_v = particles.velocities[connections.particle1[i]] - particles.velocities[connections.particle2[i]]; 
        delta_p = particles.positions[connections.particle1[i]] - particles.positions[connections.particle2[i]]; 
        delta_p_hat = glm::normalize(delta_p);
        
        spring_elongation = glm::length(delta_p) - l;
        //float sign = spring_elongation >= 0.0f ? 1.0f : -1.0f;

        force = (-k*spring_elongation - b*glm::dot(delta_v,delta_p_hat))*delta_p_hat;
        particles.forces[connections.particle1[i]] += force;
        particles.forces[connections.particle2[i]] -= force;
    }
}



void MCS::calcAccelerationOfParticle(int i){
    particles.accelerations[i] = 
        (particles.forces[i] + externalForce)/particles.masses[i] + 
        externalAcceleration;

    //Reset forces
    particles.forces[i][0] = 0.0f;
    particles.forces[i][1] = 0.0f;
    particles.forces[i][2] = 0.0f;

}

void MCS::checkCollisions(glm::vec3& p, glm::vec3& v) const{
    glm::vec3 n;
    float pos;
    for (int i = 0; i < collisionPlanes.size(); ++i){
        n = collisionPlanes[i].normal;
        pos = collisionPlanes[i].position;
        float p_dot_n = glm::dot(p,n);

        if (p_dot_n < pos){
            glm::vec3 p_offset = (p_dot_n - pos)*n;
            glm::vec3 v_parallel_n = glm::dot(v,n)*n;
            glm::vec3 v_orthogonal_n = v - v_parallel_n;
            //std::cout << "--" << std::endl;
            //std::cout << "           v: " << v[0] << " " << v[1] << " " << v[2] << std::endl;
            //std::cout << "v_parallel_n: "<< v_parallel_n[0] << " " << v_parallel_n[1] << " " << v_parallel_n[2] << std::endl;

            p -= p_offset;
            v -= v_parallel_n*(1.0f+collisionPlanes[i].elasticity);
            v -= v_orthogonal_n*collisionPlanes[i].friction;
        }
    }
}

void MCS::rotate(glm::vec3 axisOfRotation, float degrees){
    for (int i = 0; i < getNumberOfParticles(); ++i){
        //Cant rotate with glm::rotate???
    }
}

void MCS::addRotation(glm::vec3 axisOfRotation, float amount){
    glm::vec3 avgVelBefore = averageVelocity();

    glm::vec3 mid = centerOfMass();

    for (int i = 0; i < getNumberOfParticles(); ++i){
        glm::vec3 midToParticle = -particles.positions[i] + mid;
        glm::vec3 rotDir = glm::cross(midToParticle, glm::normalize(axisOfRotation));
        particles.velocities[i] += rotDir*amount;
    }

    glm::vec3 avgVelAfter = averageVelocity();
    glm::vec3 avgVelError = avgVelAfter - avgVelBefore;
    for (int i = 0; i < getNumberOfParticles(); ++i){
        particles.velocities[i] -= avgVelError;
    }
}

void MCS::setAvgPosition(glm::vec3 pos){
    glm::vec3 avgPos = averagePosition();
    glm::vec3 toAdd = pos - avgPos;
    for (int i = 0; i < getNumberOfParticles(); ++i){
        particles.positions[i] += toAdd;
    }
}

void MCS::setAvgVelocity(glm::vec3 vel){
    glm::vec3 avgVel = averageVelocity();
    glm::vec3 toAdd = vel - avgVel;
    for (int i = 0; i < getNumberOfParticles(); ++i){
        particles.velocities[i] += toAdd;
    }
}

void MCS::updateNormals(){
    int n_triangles = triangles.triangleIndices.size();
    int n_vertices = vertices.positions.size();
    
    glm::vec3 v1;
    glm::vec3 v2;
    glm::vec3 n;

    for (int i = 0; i < n_vertices; ++i){
        vertices.normals[i] = glm::vec3(0.0f,0.0f,0.0f); // Zero normals in beginning
    }
    // Calculate the triangle normals
    for(int ti = 0; ti < n_triangles; ++ti){
        v1 = vertices.positions[triangles.triangleIndices[ti].idx2] - vertices.positions[triangles.triangleIndices[ti].idx1];
        v2 = vertices.positions[triangles.triangleIndices[ti].idx3] - vertices.positions[triangles.triangleIndices[ti].idx2];

        n = glm::normalize(glm::cross(v1,v2));
        vertices.normals[triangles.triangleIndices[ti].idx1] += n;
        vertices.normals[triangles.triangleIndices[ti].idx2] += n;
        vertices.normals[triangles.triangleIndices[ti].idx3] += n;
    }
    for (int i = 0; i < n_vertices; ++i){
        vertices.normals[i] = glm::normalize(vertices.normals[i]);
    }
}

void MCS::updateVertexPositions(){
    int n_vertices = vertices.positions.size();
    for (int i = 0; i < n_vertices; ++i){
        vertices.positions[i] = particles.positions[vertices.particleIndices[i]];
    }
}

glm::vec3 MCS::centerOfMass() const{
    glm::vec3 weightedSum(0,0,0);
    float totalMass = 0;
    for (int i = 0; i < getNumberOfParticles(); ++i){
        weightedSum += particles.positions[i] * particles.masses[i];
        totalMass += particles.masses[i];
    }
    return weightedSum/totalMass;
}

glm::vec3 MCS::averagePosition() const{
    glm::vec3 sum(0,0,0);
    for (int i = 0; i < getNumberOfParticles(); ++i){
        sum += particles.positions[i];
    }
    return sum/(float)getNumberOfParticles();
}

glm::vec3 MCS::averageVelocity() const{
    glm::vec3 sum(0,0,0);
    for (int i = 0; i < getNumberOfParticles(); ++i){
        sum += particles.velocities[i];
    }
    return sum/(float)getNumberOfParticles();
}

int MCS::getNumberOfParticles() const{
    return particles.positions.size();
}

int MCS::getNumberOfConnections() const{
    return connections.lengths.size();
}

int MCS::getNumberOfVertices() const{
    return vertices.positions.size();
}

void MCS::triangle2particleIndices(int triangleIndex, int &particleIndex1, int &particleIndex2, int &particleIndex3){
 
    int Ntype0 = 2*(N_ROWS-1)*(N_COLS-1);         //back
    int Ntype1 = Ntype0;                          //front
    int Ntype2 = 2*(N_ROWS-1)*(N_STACKS-1);       //left
    int Ntype3 = Ntype2;                          //right
    int Ntype4 = 2*(N_STACKS-1)*(N_COLS-1);       //bottom
    int Ntype5 = Ntype4;                          //top
    int TotNtype = Ntype0 + Ntype1 + Ntype2 + Ntype3 + Ntype4 + Ntype5;

    assert(triangleIndex < TotNtype);

    int oneRowOfParticles = N_COLS;
    int oneRowOfParticlesLocal;
    //int oneColumnOfParticles = N_ROWS;
    int oneStackOfParticles = N_ROWS*N_COLS;
    int newTriangleIndex;

    int row_p1 = floor(triangleIndex/floor(Ntype0/(N_ROWS-1)));
    
    if(triangleIndex < Ntype0){                                     //back
        oneRowOfParticlesLocal = N_COLS;
        if(triangleIndex%2==0){     //even
            particleIndex1=triangleIndex/2+row_p1;
            particleIndex2=particleIndex1+oneRowOfParticlesLocal;
            particleIndex3=particleIndex2+1;
        }
        else{                       //odd
            particleIndex1=(triangleIndex-1)/2+row_p1;
            particleIndex2=particleIndex1+oneRowOfParticlesLocal+1;
            particleIndex3=particleIndex1+1;
        }
    }

    else if(triangleIndex < Ntype0+Ntype1){                         //front
        oneRowOfParticlesLocal = N_COLS;
        newTriangleIndex = triangleIndex - Ntype0;
        int row_p1 = floor(newTriangleIndex/floor(Ntype1/(N_ROWS-1)));

        if(newTriangleIndex%2==0){  //even
            particleIndex1=newTriangleIndex/2+row_p1;
            particleIndex2=particleIndex1+oneRowOfParticlesLocal+1;
            particleIndex3=particleIndex2-1;
        }
        else{                       //odd
            particleIndex1=(newTriangleIndex-1)/2+row_p1;
            particleIndex2=particleIndex1+1;
            particleIndex3=particleIndex2+oneRowOfParticlesLocal;
        }
        particleIndex1+=N_COLS*N_ROWS*(N_STACKS-1);
        particleIndex2+=N_COLS*N_ROWS*(N_STACKS-1);
        particleIndex3+=N_COLS*N_ROWS*(N_STACKS-1);
    }

    else if(triangleIndex < Ntype0+Ntype1+Ntype2){                  //left
        oneRowOfParticlesLocal = N_STACKS;
        newTriangleIndex=triangleIndex-(Ntype0+Ntype1);
        int row_p1 = floor(newTriangleIndex/floor(Ntype2/(N_ROWS-1)));

        if(newTriangleIndex%2==0){  //even
            particleIndex1=newTriangleIndex/2+row_p1;
            particleIndex2=particleIndex1+oneRowOfParticlesLocal+1;
            particleIndex3=particleIndex2-1;
        }
        else{                       //odd
            particleIndex1=(newTriangleIndex-1)/2+row_p1;
            particleIndex2=particleIndex1+1;
            particleIndex3=particleIndex2+oneRowOfParticlesLocal;
        }
        particleIndex1 = (particleIndex1 % N_STACKS)*oneStackOfParticles + (particleIndex1/N_STACKS)*oneRowOfParticles;
        particleIndex2 = (particleIndex2 % N_STACKS)*oneStackOfParticles + (particleIndex2/N_STACKS)*oneRowOfParticles;
        particleIndex3 = (particleIndex3 % N_STACKS)*oneStackOfParticles + (particleIndex3/N_STACKS)*oneRowOfParticles;
    }

    else if(triangleIndex < Ntype0+Ntype1+Ntype2+Ntype3){           //right
        oneRowOfParticlesLocal = N_STACKS;
        newTriangleIndex = triangleIndex - (Ntype0+Ntype1+Ntype2);
        int row_p1 = floor(newTriangleIndex/floor(Ntype3/(N_ROWS-1)));

        if(newTriangleIndex%2==0){     //even
            particleIndex1=newTriangleIndex/2+row_p1;
            particleIndex2=particleIndex1+oneRowOfParticlesLocal;
            particleIndex3=particleIndex2+1;
        }
        else{                       //odd
            particleIndex1=(newTriangleIndex-1)/2+row_p1;
            particleIndex2=particleIndex1+oneRowOfParticlesLocal+1;
            particleIndex3=particleIndex1+1;
        }
        particleIndex1 = (particleIndex1 % N_STACKS)*oneStackOfParticles + (particleIndex1/N_STACKS)*oneRowOfParticles + oneRowOfParticles-1;
        particleIndex2 = (particleIndex2 % N_STACKS)*oneStackOfParticles + (particleIndex2/N_STACKS)*oneRowOfParticles + oneRowOfParticles-1;
        particleIndex3 = (particleIndex3 % N_STACKS)*oneStackOfParticles + (particleIndex3/N_STACKS)*oneRowOfParticles + oneRowOfParticles-1;
    }

    else if(triangleIndex < Ntype0+Ntype1+Ntype2+Ntype3+Ntype4){    //bottom
        oneRowOfParticlesLocal = N_COLS;
        newTriangleIndex = triangleIndex - (Ntype0+Ntype1+Ntype2+Ntype3);
        int row_p1 = floor(newTriangleIndex/floor(Ntype4/(N_STACKS-1)));
        if(newTriangleIndex%2==0){  //even
            particleIndex1=newTriangleIndex/2+row_p1;
            particleIndex2=particleIndex1+oneRowOfParticlesLocal+1;
            particleIndex3=particleIndex2-1;
        }
        else{                       //odd
            particleIndex1=(newTriangleIndex-1)/2+row_p1;
            particleIndex2=particleIndex1+1;
            particleIndex3=particleIndex2+oneRowOfParticlesLocal;
        }
        particleIndex1 = (particleIndex1 / N_COLS)*oneStackOfParticles + particleIndex1 % N_COLS;
        particleIndex2 = (particleIndex2 / N_COLS)*oneStackOfParticles + particleIndex2 % N_COLS;
        particleIndex3 = (particleIndex3 / N_COLS)*oneStackOfParticles + particleIndex3 % N_COLS;
    }

    else if(triangleIndex < TotNtype){                              //top
        oneRowOfParticlesLocal = N_COLS;
        newTriangleIndex = triangleIndex - (Ntype0+Ntype1+Ntype2+Ntype3+Ntype4);
        int row_p1 = floor(newTriangleIndex/floor(Ntype5/(N_STACKS-1)));
        if(newTriangleIndex%2==0){     //even
            particleIndex1=newTriangleIndex/2+row_p1;
            particleIndex2=particleIndex1+oneRowOfParticlesLocal;
            particleIndex3=particleIndex2+1;
        }
        else{                       //odd
            particleIndex1=(newTriangleIndex-1)/2+row_p1;
            particleIndex2=particleIndex1+oneRowOfParticlesLocal+1;
            particleIndex3=particleIndex1+1;
        }
        particleIndex1 = (particleIndex1 / N_COLS)*oneStackOfParticles + particleIndex1 % N_COLS + (N_ROWS-1)*N_COLS;
        particleIndex2 = (particleIndex2 / N_COLS)*oneStackOfParticles + particleIndex2 % N_COLS + (N_ROWS-1)*N_COLS;
        particleIndex3 = (particleIndex3 / N_COLS)*oneStackOfParticles + particleIndex3 % N_COLS + (N_ROWS-1)*N_COLS;
    }
}

void MCS::vertex2particleIndex(int vertexIndex, int &particleIndex){
    int Ntype0 = N_ROWS * N_COLS;         //back
    int Ntype1 = Ntype0;                          //front
    int Ntype2 = N_ROWS * N_STACKS;       //left
    int Ntype3 = Ntype2;                          //right
    int Ntype4 = N_STACKS * N_COLS;       //bottom
    int Ntype5 = Ntype4;                          //top
    int TotNtype = Ntype0 + Ntype1 + Ntype2 + Ntype3 + Ntype4 + Ntype5;

    assert(vertexIndex < TotNtype);

    int oneRowOfParticles = N_COLS;
    int oneRowOfParticlesLocal;
    int oneStackOfParticles = N_ROWS*N_COLS;
    int newVertexIndex;

    if(vertexIndex < Ntype0){                                         //back
        particleIndex = vertexIndex;
    }
    else if(vertexIndex < Ntype0 + Ntype1){                           //front
        newVertexIndex = vertexIndex - Ntype0;
        particleIndex = newVertexIndex + N_COLS*N_ROWS*(N_STACKS-1);
    }
    else if(vertexIndex < Ntype0 + Ntype1 + Ntype2){                  //left
        newVertexIndex = vertexIndex - Ntype0 - Ntype1;
        particleIndex = (newVertexIndex % N_STACKS)*oneStackOfParticles + (newVertexIndex/N_STACKS)*oneRowOfParticles;
    }
    else if(vertexIndex < Ntype0 + Ntype1 + Ntype2 + Ntype3){         //right
        newVertexIndex = vertexIndex - Ntype0 - Ntype1 - Ntype2;
        particleIndex = (newVertexIndex % N_STACKS)*oneStackOfParticles + (newVertexIndex/N_STACKS)*oneRowOfParticles + oneRowOfParticles-1;
    }
    else if(vertexIndex < Ntype0 + Ntype1 + Ntype2 + Ntype3 + Ntype4){//bottom
        newVertexIndex = vertexIndex - Ntype0 - Ntype1 - Ntype2 - Ntype3;
        particleIndex = (newVertexIndex / N_COLS)*oneStackOfParticles + newVertexIndex % N_COLS;
    }
    else if(vertexIndex < TotNtype){                                  //top
        newVertexIndex = vertexIndex - Ntype0 - Ntype1 - Ntype2 - Ntype3 - Ntype4;
        particleIndex = (newVertexIndex / N_COLS)*oneStackOfParticles + newVertexIndex % N_COLS + (N_ROWS-1)*N_COLS;
    }
}

void MCS::triangle2vertexIndices(int triangleIndex, int &vertexIndex1, int &vertexIndex2, int &vertexIndex3){
    int Ntype0 = 2*(N_ROWS-1)*(N_COLS-1);         //back
    int Ntype1 = Ntype0;                          //front
    int Ntype2 = 2*(N_ROWS-1)*(N_STACKS-1);       //left
    int Ntype3 = Ntype2;                          //right
    int Ntype4 = 2*(N_STACKS-1)*(N_COLS-1);       //bottom
    int Ntype5 = Ntype4;                          //top
    int TotNtype = Ntype0 + Ntype1 + Ntype2 + Ntype3 + Ntype4 + Ntype5;

    int Ntype_verts0 = N_ROWS * N_COLS;         //back
    int Ntype_verts1 = Ntype_verts0;                          //front
    int Ntype_verts2 = N_ROWS * N_STACKS;       //left
    int Ntype_verts3 = Ntype_verts2;                          //right
    int Ntype_verts4 = N_STACKS * N_COLS;       //bottom
    int Ntype_verts5 = Ntype_verts4;                          //top
    int TotNtype_verts = Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3 + Ntype_verts4 + Ntype_verts5;

    assert(triangleIndex < TotNtype);

    int oneRowOfParticles = N_COLS;
    int oneRowOfParticlesLocal;
    //int oneColumnOfParticles = N_ROWS;
    int oneStackOfParticles = N_ROWS*N_COLS;
    int newTriangleIndex;

    int row_p1 = floor(triangleIndex/floor(Ntype0/(N_ROWS-1)));

    if(triangleIndex < Ntype0){                                         //back
        oneRowOfParticlesLocal = N_COLS;
        if(triangleIndex%2==0){     //even
            vertexIndex1=triangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal;
            vertexIndex3=vertexIndex2+1;
        }
        else{                       //odd
            vertexIndex1=(triangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex1+1;
        }
    }
    else if(triangleIndex < Ntype0 + Ntype1){                           //front
        oneRowOfParticlesLocal = N_COLS;
        newTriangleIndex = triangleIndex - Ntype0;
        int row_p1 = floor(newTriangleIndex/floor(Ntype1/(N_ROWS-1)));

        if(newTriangleIndex%2==0){  //even
            vertexIndex1=newTriangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex2-1;
        }
        else{                       //odd
            vertexIndex1=(newTriangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+1;
            vertexIndex3=vertexIndex2+oneRowOfParticlesLocal;
        }
        vertexIndex1 += Ntype_verts0;
        vertexIndex2 += Ntype_verts0;
        vertexIndex3 += Ntype_verts0;
    }
    else if(triangleIndex < Ntype0+Ntype1+Ntype2){                  //left
        oneRowOfParticlesLocal = N_STACKS;
        newTriangleIndex=triangleIndex-(Ntype0+Ntype1);
        int row_p1 = floor(newTriangleIndex/floor(Ntype2/(N_ROWS-1)));

        if(newTriangleIndex%2==0){  //even
            vertexIndex1=newTriangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex2-1;
        }
        else{                       //odd
            vertexIndex1=(newTriangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+1;
            vertexIndex3=vertexIndex2+oneRowOfParticlesLocal;
        }
        vertexIndex1 += Ntype_verts0 + Ntype_verts1;
        vertexIndex2 += Ntype_verts0 + Ntype_verts1;
        vertexIndex3 += Ntype_verts0 + Ntype_verts1;
    }
    else if(triangleIndex < Ntype0+Ntype1+Ntype2+Ntype3){           //right
        oneRowOfParticlesLocal = N_STACKS;
        newTriangleIndex = triangleIndex - (Ntype0+Ntype1+Ntype2);
        int row_p1 = floor(newTriangleIndex/floor(Ntype3/(N_ROWS-1)));

        if(newTriangleIndex%2==0){     //even
            vertexIndex1=newTriangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal;
            vertexIndex3=vertexIndex2+1;
        }
        else{                       //odd
            vertexIndex1=(newTriangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex1+1;
        }
        vertexIndex1 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2;
        vertexIndex2 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2;
        vertexIndex3 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2;
    }
    else if(triangleIndex < Ntype0+Ntype1+Ntype2+Ntype3+Ntype4){    //bottom
        oneRowOfParticlesLocal = N_COLS;
        newTriangleIndex = triangleIndex - (Ntype0+Ntype1+Ntype2+Ntype3);
        int row_p1 = floor(newTriangleIndex/floor(Ntype4/(N_STACKS-1)));
        if(newTriangleIndex%2==0){  //even
            vertexIndex1=newTriangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex2-1;
        }
        else{                       //odd
            vertexIndex1=(newTriangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+1;
            vertexIndex3=vertexIndex2+oneRowOfParticlesLocal;
        }
        vertexIndex1 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3;
        vertexIndex2 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3;
        vertexIndex3 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3;
    }
    else if(triangleIndex < TotNtype){                                  //top
        oneRowOfParticlesLocal = N_COLS;
        newTriangleIndex = triangleIndex - (Ntype0+Ntype1+Ntype2+Ntype3+Ntype4);
        int row_p1 = floor(newTriangleIndex/floor(Ntype5/(N_STACKS-1)));
        if(newTriangleIndex%2==0){     //even
            vertexIndex1=newTriangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal;
            vertexIndex3=vertexIndex2+1;
        }
        else{                       //odd
            vertexIndex1=(newTriangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex1+1;
        }
        vertexIndex1 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3 + Ntype_verts4;
        vertexIndex2 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3 + Ntype_verts4;
        vertexIndex3 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3 + Ntype_verts4;
    }
}
