#include "MCS.h"


//Constructor
MCS::MCS(const int n_rows, const int n_cols, const int n_stacks):   
    N_ROWS(n_rows),N_COLS(n_cols),N_STACKS(n_stacks),
    particles(n_rows*n_cols*n_stacks)
{
	initParticles();
    initConnections();
}


void MCS::initParticles(){
    for (int i = 0; i < N_COLS*N_ROWS*N_STACKS; ++i){
        float x = i%N_COLS;
        float y = (i/N_COLS)%N_ROWS;
        float z = i/(N_COLS*N_ROWS);
        particles[i] = Particle(1.0f, glm::vec3(x,y,z)); 
    }

    //particles[0].storeForce(glm::vec3(0,50000,50000));

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

    connections = std::vector<Connection>(numberOfConnections);

    // Calculate connections
    int p_index1;
    int p_index2;
    for (int i = 0; i < numberOfConnections; ++i){
        connection2massIndices3D(i, p_index1, p_index2, N_ROWS, N_COLS, N_STACKS);
        connections[i] = Connection(&particles[p_index1], &particles[p_index2]);

        connections[i].setSpringConstant(500.f);
        connections[i].setDamperConstant(5.0f);
        connections[i].setConnectionLength(1.0f);
    }

    //Set length of 2D-diagonal springs to sqrt(2)
    for (int i = startOfConnectionOfType[3]; i < startOfConnectionOfType[9]; ++i){
        connections[i].setConnectionLength(sqrt(2.0f));
    }

    //Set length of 3D-diagonal springs to sqrt(3)
    for (int i = startOfConnectionOfType[9]; i < numberOfConnections; ++i){
        connections[i].setConnectionLength(sqrt(3.0f));
    }
}

void MCS::update(float dt){
    for (int i = 0; i < connections.size(); ++i){
        connections[i].applyForcesToConnectedParticles(dt);
    }

    for (int i = 0; i < particles.size(); ++i){
        particles[i].applyForce(dt);
    }
}

void MCS::rotate(glm::vec3 axisOfRotation, float degrees){

    for (int i = 0; i < particles.size(); ++i){
        Particle& p = particles[i];
        //Cant rotate with glm::rotate???
        
    }
}

void MCS::addRotation(glm::vec3 axisOfRotation, float amount){
    glm::vec3 avgVelBefore = averageVelocity();

    glm::vec3 mid = centerOfMass();
    for (int i = 0; i < particles.size(); ++i){
        Particle& p = particles[i];
        glm::vec3 midToParticle = -p.readPosition() + mid;
        glm::vec3 rotDir = glm::cross(midToParticle, glm::normalize(axisOfRotation));
        p.writeVelocity(p.readVelocity() + rotDir*amount);
    }

    glm::vec3 avgVelAfter = averageVelocity();
    glm::vec3 avgVelError = avgVelAfter - avgVelBefore;
    for (int i = 0; i < particles.size(); ++i){
        //particles[i].writeVelocity(particles[i].readVelocity()-avgVelError);
    }
}

void MCS::setAvgPosition(glm::vec3 pos){
    glm::vec3 avgPos = averagePosition();
    glm::vec3 toAdd = pos - avgPos;
    for (int i = 0; i < particles.size(); ++i){
        particles[i].writePosition(particles[i].readPosition() + toAdd);
    }
}

void MCS::setAvgVelocity(glm::vec3 vel){
    glm::vec3 avgVel = averageVelocity();
    glm::vec3 toAdd = vel - avgVel;
    for (int i = 0; i < particles.size(); ++i){
        particles[i].writeVelocity(particles[i].readVelocity() + toAdd);
    }
}

glm::vec3 MCS::centerOfMass() const{
    glm::vec3 weightedSum(0,0,0);
    float totalMass;
    for (int i = 0; i < particles.size(); ++i){
        weightedSum += particles[i].readPosition()*particles[i].getMass();
        totalMass += particles[i].getMass();
    }
    return weightedSum/totalMass;
}

glm::vec3 MCS::averagePosition() const{
    glm::vec3 sum(0,0,0);
    for (int i = 0; i < particles.size(); ++i){
        sum += particles[i].readPosition();
    }
    return sum/(float)particles.size();
}

glm::vec3 MCS::averageVelocity() const{
    glm::vec3 sum(0,0,0);
    for (int i = 0; i < particles.size(); ++i){
        sum += particles[i].readVelocity();
    }
    return sum/(float)particles.size();
}

//Get-functions
Particle& MCS::getParticle(int index){
    return particles[index];
}

const Connection& MCS::getConnection(int index) const{
    return connections[index];
}

int MCS::getNumberOfParticles(){
    return particles.size();
}

int MCS::getNumberOfConnections(){
    return connections.size();
}
