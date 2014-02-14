#include "Models.h"


//Constructor
MCS::MCS(const int n_rows, const int n_cols, const int n_stacks)
        : N_ROWS(n_rows),N_COLS(n_cols), N_STACKS(n_stacks),
          N_TYPE1((N_ROWS-1)*N_COLS), N_TYPE2((N_ROWS-1)*(N_COLS-1)),
          N_TYPE3(N_ROWS*(N_COLS-1)), N_TYPE4(N_TYPE2),
          N_PARTICLES(N_ROWS*N_COLS*N_STACKS), N_CONNECTIONS(N_TYPE1+N_TYPE2+N_TYPE3+N_TYPE4)
{

    particles = new Particle[N_PARTICLES];
    
	setStartingValues();
    connections = new Connection[N_PARTICLES];
/* TODO
	// Calculate connections
    for (int i = 0; i < N_CONNECTIONS; ++i){
        connection2massIndices(i, connected_particles[i][0], connected_particles[i][1], N_ROWS, N_COLS, N_STACKS);
    }*/

}

//Destructor
MCS::~MCS(){
    delete[] particles;
    delete[] connections;
}

void MCS::setStartingValues(){
// Set starting values to masses, positions velocities and forces
    for (int i = 0; i < N_PARTICLES; ++i)
    {
        int row = i%N_COLS;                         //x
        int col = floor(i/N_COLS);                  //y
        int stack = floor(i/(N_COLS*N_ROWS));       //z
        particles[i] = Particle(1.0f, glm::vec3(row,col,stack));    // starting position from 3D index, vel. = 0
    }
}

//Get-functions
int MCS::getCols(){return(N_COLS);}
int MCS::getRows(){return(N_ROWS);}
int MCS::getStack(){return(N_STACKS);}
int MCS::getParticles(){return(N_PARTICLES);}
int MCS::getConnections(){return(N_CONNECTIONS);}

/*
//constructor
MSDS::MSDS(float spring_const, float damper_const, float spring_l, n_rows, n_cols) : MCS(n_rows, n_cols);{
	// Set values for springs, dampers, and lengths
    for (int i = 0; i < N_CONNECTIONS; ++i)
    {
        spring_constants[i] = spring_const;
        damper_constants[i] = damper_const;
        spring_lengths[i] = spring_l;
    }

    // Set type_2 connections to legnth sqrt(2)
    for (int i = N_TYPE1; i < N_TYPE1+N_TYPE2; ++i)
    {
        spring_lengths[i] = sqrt(2);
    }

    // Set type_4 connections to legnth sqrt(2)
    for (int i = N_TYPE1+N_TYPE2+N_TYPE3; i < N_CONNECTIONS; ++i)
    {
        spring_lengths[i] = sqrt(2);
    }
}

//destructor
MSDS::~MSDS(){
	delete[] spring_constants;
	delete[] damper_constants;
	delete[] spring_lengths;
}
*/