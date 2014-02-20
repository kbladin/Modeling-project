#include "MCS.h"


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

	// Calculate connections
    int connected_particle1, connected_particle2;
    /* TODO
    for (int i = 0; i < N_CONNECTIONS; ++i){
        connection2massIndices(i, connected_particle1, connected_particle2, N_ROWS, N_COLS, N_STACKS);
        connections[i] = new Connection(&particles[connected_particle1], &particles[connected_particle2]);

        //connections[i]._p1 = &particles[connected_particles1];
        //connections[i]._p2 = &particles[connected_particles2];
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
