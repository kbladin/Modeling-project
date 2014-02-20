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

        void rotate(glm::vec3 axisOfRotation, float amount);
        void addRotation(glm::vec3 axisOfRotation, float amount);
        
        glm::vec3 centerOfMass() const;
        glm::vec3 averageVelocity() const;

        //The dimensions variables for the MCS
        const int N_ROWS;
        const int N_COLS;
        const int N_STACKS;

        const Particle& getParticle(int index) const;
        const Connection& getConnection(int index) const;

        //Get-functions to access the protected variables in the MCS class
        int getNumberOfParticles();
        int getNumberOfConnections();

        //The number of connections of each type/direction
        
        int numberOfConnectionsOfType[13];
        int startOfConnectionOfType[13];
        

    protected:
        friend void testMCS();

        void initParticles();
        void initConnections();
        
        std::vector<Particle> particles;
        std::vector<Connection> connections;
};
#endif