#include "MCS.h"


//Constructor
MCS::MCS(const int n_rows, const int n_cols, const int n_stacks):   
    N_ROWS(n_rows),N_COLS(n_cols),N_STACKS(n_stacks)
{
	initParticles();
    initConnections();
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
        connections.springConstants[i] = 500.0f;
        connections.damperConstants[i] = 5.0f;
        connections.particle1[i] = p_index1;
        connections.particle2[i] = p_index2;
    }

    //Set length of 2D-diagonal springs to sqrt(2)
    for (int i = startOfConnectionOfType[3]; i < startOfConnectionOfType[9]; ++i){
        connections.lengths[i] = sqrt(2.0f);
    }

    //Set length of 3D-diagonal springs to sqrt(3)
    for (int i = startOfConnectionOfType[9]; i < numberOfConnections; ++i){
        connections.lengths[i] = sqrt(3.0f);
    }
}

void MCS::update(float dt){
/*
    for (int i = 0; i < connections.lengths.size(); ++i){
        connections[i].applyForcesToConnectedParticles(dt);
    }
    

    for (int i = 0; i < getNumberOfParticles(); ++i){
        particles[i].applyForce(dt);
    }
*/
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
