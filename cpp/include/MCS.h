#ifndef MCS_H
#define MCS_H

#include <vector>
#include <glm/glm.hpp>
#include "connection2massindices.h"

/** Classes **/

typedef struct{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    std::vector<glm::vec3> accelerations;
    std::vector<glm::vec3> forces;
    std::vector<float> masses;
} Particles;

typedef struct{
    std::vector<float> lengths;
    std::vector<float> springConstants;
    std::vector<float> damperConstants;
    std::vector<int> particle1;
    std::vector<int> particle2;
} Connections;

// The MCS - Mass Connection System
class MCS{

    public:
        // Constructor
        MCS(const int n_rows, const int n_cols, const int n_stacks);

        void update(float dt);

        void rotate(glm::vec3 axisOfRotation, float amount);
        void addRotation(glm::vec3 axisOfRotation, float amount);
        
        void setAvgPosition(glm::vec3 pos);
        void setAvgVelocity(glm::vec3 pos);

        glm::vec3 centerOfMass() const;
        glm::vec3 averagePosition() const;
        glm::vec3 averageVelocity() const;

        //Get-functions to access the protected variables in the MCS class
        int getNumberOfParticles() const;
        int getNumberOfConnections() const;


        Particles particles;
        Connections connections;
        
        
        //The number of connections of each type/direction
        int numberOfConnectionsOfType[13];
        int startOfConnectionOfType[13];
        
        //The dimensions variables for the MCS
        const int N_ROWS;
        const int N_COLS;
        const int N_STACKS;

        friend void testMCS();


    private:
        void calcConnectionForcesOnParticles();
        void applyForcesOnParticles(float dt, glm::vec3 externalForce = glm::vec3(0,0,0), glm::vec3 externalAcceleration = glm::vec3(0,0,0));
        //void connection2massIndices3D(const int connection_index, int &mass_index1, int &mass_index2, const int n_rows, const int n_cols, const int n_stacks);

        // Calculate the stack in which the first mass (not necessary mass 1) of the pair is positioned
        //int stackOfFirstMass(const int connection_index, const int prev_num_springs, const int one_stack_of_connections);

        // Calculate the row in which first mass (not necessary mass 1) of the pair is positioned
        //int rowOfFirstMass(const int connection_index, const int prev_num_springs, const int one_row_of_connections, const int one_stack_of_connections);
        void initParticles();
        void initConnections();
};
#endif