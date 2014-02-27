#include "MCS.h"

//Constructor
MCS::MCS(const int n_rows, const int n_cols, const int n_stacks):   
    N_ROWS(n_rows),N_COLS(n_cols),N_STACKS(n_stacks)
{
	initParticles();
    initConnections();
    initTriangles();
}

void MCS::initParticles(){

    int numberOfParticles = N_COLS*N_ROWS*N_STACKS;

    particles.positions = std::vector<glm::vec3>(numberOfParticles);
    particles.velocities = std::vector<glm::vec3>(numberOfParticles);
    particles.accelerations = std::vector<glm::vec3>(numberOfParticles);
    particles.forces = std::vector<glm::vec3>(numberOfParticles);
    particles.masses = std::vector<float>(numberOfParticles);

    for (int i = 0; i < numberOfParticles; ++i){
        float x = i%N_COLS;
        float y = (i/N_COLS)%N_ROWS;
        float z = i/(N_COLS*N_ROWS);

        particles.positions[i] = glm::vec3(x,y,z);
        particles.velocities[i]  = glm::vec3(0.f,0.f,0.f);
        particles.accelerations[i] = glm::vec3(0.f,0.f,0.f);
        particles.forces[i] = glm::vec3(0.f,0.f,0.f);
        particles.masses[i] = 1.0f;    
    } 
}

void MCS::initConnections(){
    numberOfConnectionsOfType[0] = N_ROWS*(N_COLS-1)*N_STACKS;
    numberOfConnectionsOfType[1] = (N_ROWS-1)*N_COLS*N_STACKS;
    numberOfConnectionsOfType[2] = N_ROWS*N_COLS*(N_STACKS-1);
    numberOfConnectionsOfType[3] = (N_ROWS-1)*(N_COLS-1)*N_STACKS;
    numberOfConnectionsOfType[4] = (N_ROWS-1)*(N_COLS-1)*N_STACKS;
    numberOfConnectionsOfType[5] = N_ROWS*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[6] = N_ROWS*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[7] = (N_ROWS-1)*N_COLS*(N_STACKS-1);
    numberOfConnectionsOfType[8] = (N_ROWS-1)*N_COLS*(N_STACKS-1);
    numberOfConnectionsOfType[9] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[10] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[11] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[12] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);

    startOfConnectionOfType[0] = 0;
    int numberOfConnections = numberOfConnectionsOfType[0];
    for (int i = 1; i < 13; ++i){
        startOfConnectionOfType[i] = startOfConnectionOfType[i-1] + numberOfConnectionsOfType[i-1];
        numberOfConnections += numberOfConnectionsOfType[i];
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
        connections.springConstants[i] = 700.0f;
        connections.damperConstants[i] = 5.0f;
        connections.particle1[i] = p_index1;
        connections.particle2[i] = p_index2;
    }

    //Set length of 2D-diagonal springs to sqrt(2)
    for (int i = startOfConnectionOfType[3]; i < startOfConnectionOfType[9]; ++i){
        connections.lengths[i] *= sqrt(2.0f);
    }

    //Set length of 3D-diagonal springs to sqrt(3)
    for (int i = startOfConnectionOfType[9]; i < numberOfConnections; ++i){
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
    const int n_plane1 = 2*((N_ROWS-1)*(N_COLS-1));
    const int n_plane2 = n_plane1;
    const int n_plane3 = 2*((N_ROWS-1)*(N_STACKS-1));
    const int n_plane4 = n_plane3;
    const int n_plane5 = 2*((N_COLS-1)*(N_STACKS-1));
    const int n_plane6 = n_plane5;

    const int n_triangles =
        n_plane1 + n_plane2 + 
        n_plane3 + n_plane4 + 
        n_plane5 + n_plane6;

    triangles.triangleIndices = std::vector<IndexedTriangle>(n_triangles);
    for(int ti = 0; ti < n_triangles; ++ti){
        
        triangle2particleIndices(
            ti,
            triangles.triangleIndices[ti].idx1,
            triangles.triangleIndices[ti].idx2,
            triangles.triangleIndices[ti].idx3);
    }
}

void MCS::update(float dt, glm::vec3 externalAcceleration){

    std::vector<float> w;
    w.push_back(1.0f);
    w.push_back(3.0f);
    w.push_back(3.0f);
    w.push_back(1.0f);

    float wSum = 0.0f;
    for (int i = 0; i < w.size(); ++i){
        wSum += w[i];
    }

    //Declare memory
    int p1,p2;

    std::vector<glm::vec3> delta_v_offsets(getNumberOfConnections(),glm::vec3(0,0,0));
    std::vector<glm::vec3> delta_p_offsets(getNumberOfConnections(),glm::vec3(0,0,0));
    std::vector<std::vector<glm::vec3> > ka(w.size());
    std::vector<std::vector<glm::vec3> > kv(w.size());
    for (int i = 0; i < w.size(); ++i){
        ka[i] = std::vector<glm::vec3>(getNumberOfParticles(), glm::vec3(0,0,0));
        kv[i] = std::vector<glm::vec3>(getNumberOfParticles(), glm::vec3(0,0,0));
    }

    /* ------------------------- */


    // RUNGE KUTTA 4
    for (int k = 0; k < w.size(); ++k){

        //Calc offsets
        if (k>0){
            for (int i = 0; i < getNumberOfConnections(); ++i){
                p1 = connections.particle1[i];
                p2 = connections.particle2[i];
                delta_v_offsets[i] = (ka[k-1][p1] - ka[k-1][p2]) * dt / w[k];
                delta_p_offsets[i] = delta_v_offsets[i] * dt / w[k];
            }
        }

        //Calc accelerations

        calcConnectionForcesOnParticles(delta_v_offsets, delta_p_offsets);
        calcAccelerationOfParticles(glm::vec3(0,-1,0)*9.82f);

        for (int i = 0; i < getNumberOfParticles(); ++i){
            ka[k][i] = particles.accelerations[i];
            kv[k][i] = particles.velocities[i] + ka[k][i] * dt;
        }
    }

    //Update velocities and positions
    glm::vec3 new_position(0,0,0);
    glm::vec3 new_velocity(0,0,0);
    glm::vec3 delta_v(0,0,0);
    glm::vec3 delta_p(0,0,0);
    for (int i = 0; i < getNumberOfParticles(); ++i){

        //Calc new position and velocity
        for (int wi = 0; wi < w.size(); ++wi){
            delta_v += ka[wi][i] * w[wi];
            delta_p += kv[wi][i] * w[wi];
        }
        delta_v *= dt/wSum;
        delta_p *= dt/wSum;

        new_velocity = particles.velocities[i] + delta_v;
        new_position = particles.positions[i] + delta_p;
        

        //Check collisions with collision planes
        checkCollisions(new_position, new_velocity);

        //Set new position and velocity
        particles.velocities[i] = new_velocity;
        particles.positions[i] = new_position;
    }
    //glm::vec3 a = particles.positions[0];
    //std::cout << "pos: " << a[0] << ", " << a[1] << ", " << a[2] << std::endl;
    //updateParticles(dt);
}


void MCS::calcConnectionForcesOnParticles(std::vector<glm::vec3> delta_v_offset, std::vector<glm::vec3> delta_p_offset){
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

void MCS::calcAccelerationOfParticles(glm::vec3 externalAcceleration, glm::vec3 externalForce){
    for (int i = 0; i < getNumberOfParticles(); ++i){
        particles.accelerations[i] = 
            (particles.forces[i] + externalForce)/particles.masses[i] + 
            externalAcceleration;

        //Reset forces
        particles.forces[i][0] = 0.0f;
        particles.forces[i][1] = 0.0f;
        particles.forces[i][2] = 0.0f;
    }
}


void MCS::updateParticles(float dt){
    //Allocate memory for two new position and velocity
    glm::vec3 new_position;
    glm::vec3 new_velocity;


    for (int i = 0; i < getNumberOfParticles(); ++i){
        //Calc new position and velocity
        new_velocity = particles.velocities[i] + particles.accelerations[i] * dt;
        new_position = particles.positions[i] + new_velocity * dt;

        //Check collisions with collision planes
        checkCollisions(new_position, new_velocity);

        //Set new position and velocity
        particles.velocities[i] = new_velocity;
        particles.positions[i] = new_position;
    }
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

glm::vec3 MCS::centerOfMass() const{
    glm::vec3 weightedSum(0,0,0);
    float totalMass;
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
    if(triangleIndex < Ntype0){
        oneRowOfParticlesLocal = N_COLS;
        int row_p1 = floor(triangleIndex/floor(Ntype0/(N_ROWS-1)));//back

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

