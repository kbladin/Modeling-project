#ifndef MCS_H
#define MCS_H

#include <vector>
#include <glm/glm.hpp>
#include "connection2massindices.h"

/** Classes **/
typedef struct{
    int idx1;
    int idx2;
    int idx3;
} IndexedTriangle;

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

typedef struct{
    glm::vec3 normal;
    float position;
    float elasticity;
    float friction;
} CollisionPlane;

typedef struct {
    std::vector<IndexedTriangle> triangleIndices;
} Triangles;


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

        void addCollisionPlane(glm::vec3 normal, 
                               float position, 
                               float elasticity = 0.0f,
                               float friction = 0.0f);

        Triangles triangles;
        
        
        //The number of connections of each type/direction
        int numberOfConnectionsOfType[13];
        int startOfConnectionOfType[13];
        
        //The dimensions variables for the MCS
        const int N_ROWS;
        const int N_COLS;
        const int N_STACKS;

        //Applied during update
        glm::vec3 externalAcceleration;
        glm::vec3 externalForce;

        friend void testMCS();

    private:
        void calcConnectionForcesOnParticles(
            std::vector<glm::vec3> delta_v_offset,
            std::vector<glm::vec3> delta_p_offset);
        void calcAccelerationOfParticle(int i);
        void updateParticles(float dt);
        void checkCollisions(glm::vec3& pos, glm::vec3& vel) const;
        
        
        void initParticles();
        void initConnections();

        

        std::vector<CollisionPlane> collisionPlanes;

        void initTriangles();
        void triangle2particleIndices(int triangleIndex, int &particleIndex1, int &particleIndex2, int &particleIndex3);


};
#endif