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
    // Set starting values to masses, positions velocities and forces
    int numberOfParticles = N_COLS*N_ROWS*N_STACKS;
    for (int i = 0; i < numberOfParticles; ++i)
    {
        int row = i%N_COLS;                         //x
        int col = (i/N_COLS)%N_ROWS;              //y
        int stack = i/(N_COLS*N_ROWS);              //z
        // starting position from 3D index, vel. = 0
        particles[i] = Particle(1.0f, glm::vec3(row,col,stack)); 
    }

    //particles[0].storeForce(glm::vec3(0,0,1000));
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

    std::vector<Connection> c_tmp(numberOfConnections);
    connections = c_tmp;

    // Calculate connections
    int p_index1;
    int p_index2;
    for (int i = 0; i < numberOfConnections; ++i){
        connection2massIndices3D(i, p_index1, p_index2, N_ROWS, N_COLS, N_STACKS);
        connections[i] = Connection(&particles[p_index1], &particles[p_index2]);
        connections[i].setSpringConstant(100.f);
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
    for (int i = 0; i < connections.size(); ++i)
    {
        connections[i].applyForcesToConnectedParticles(dt);
    }

    for (int i = 0; i < particles.size(); ++i)
    {
        particles[i].applyForce(dt);
    }
}

//Get-functions
const Particle& MCS::getParticle(int index) const{
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
