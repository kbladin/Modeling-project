#ifndef MCS_H
#define MCS_H

#include <vector>
#include <glm/glm.hpp>
#include "Particle.h"
#include "Connection.h"
#include "connection2massindices.h"

/** Classes **/

// The MCS - Mass Connection System
class MCS{

    public:
        // Constructor
        MCS(const int n_rows, const int n_cols, const int n_stacks);

        void update(float dt);

        //The dimensions variables for the MCS
        const int N_ROWS;
        const int N_COLS;
        const int N_STACKS;

        //Get-functions to access the protected variables in the MCS class
        int getParticles();
        int getConnections();

        

    protected:
        friend void testMCS();

        
        

        //The number of connections of each type/direction
        const int N_TYPE1;
        const int N_TYPE2;
        const int N_TYPE3;
        const int N_TYPE4;

        //The total number of particles and connections
        const int N_PARTICLES;
        const int N_CONNECTIONS;

        void initParticles();
        void initConnections();
        
        std::vector<Particle> particles;
        std::vector<Connection> connections;

};
#endif