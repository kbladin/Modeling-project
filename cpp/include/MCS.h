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

        const Particle& getParticle(int index) const;
        const Connection& getConnection(int index) const;

        //Get-functions to access the protected variables in the MCS class
        int numberOfParticles();
        int numberOfConnections();

        //The number of connections of each type/direction
        const int N_TYPE0;
        const int N_TYPE1;
        const int N_TYPE2;
        const int N_TYPE3;
        const int N_TYPE4;
        const int N_TYPE5;
        const int N_TYPE6;
        const int N_TYPE7;
        const int N_TYPE8;
        const int N_TYPE9;
        const int N_TYPE10;
        const int N_TYPE11;
        const int N_TYPE12;

        

    protected:
        friend void testMCS();

        


        //The total number of particles and connections
        const int N_PARTICLES;
        const int N_CONNECTIONS;

        void initParticles();
        void initConnections();
        
        std::vector<Particle> particles;
        std::vector<Connection> connections;
};
#endif