#ifndef MCS_H
#define MCS_H

#include <vector>
#include <glm/glm.hpp>
#include "ElasticMaterials_lib/conversions.h"
#include "ElasticMaterials_lib/HelperClasses.h"
#include "debugtools.h"

// The MCS - Mass Connection System
class MCS{
    public:
        MCS(const int n_rows, const int n_cols, const int n_stacks);

        // Set-functions
        void rotate(glm::vec3 axisOfRotation, float amount);
        void addRotation(glm::vec3 axisOfRotation, float amount);
        void setAvgPosition(glm::vec3 pos);
        void setAvgVelocity(glm::vec3 pos);

        void updateNormals();
        void updateVertexPositions();

        void addCollisionPlane(glm::vec3 normal,
                           float position,
                           float elasticity = 0.0f,
                           float friction = 0.0f);
        void freeze();

        void calcConnectionForcesOnParticles();
        void calcConnectionForcesOnParticles(
                                        const std::vector<glm::vec3>& delta_v_offset,
                                        const std::vector<glm::vec3>& delta_p_offset);
        void calcAccelerationOfParticle(int i);
        void checkCollisions(glm::vec3& pos, glm::vec3& vel) const;

        // Get-functions
        glm::vec3 centerOfMass() const;
        glm::vec3 averagePosition() const;
        glm::vec3 averageVelocity() const;

        //Get-functions to access the protected variables in the MCS class
        int getNumberOfParticles() const;
        int getNumberOfConnections() const;
        int getNumberOfVertices() const;


        // Public members
        Particles particles;
        Connections connections;
        Vertices vertices;
        Triangles triangles;
    
        ParticleInterval INTERVAL_ALL;
        ParticleInterval INTERVAL_FIRST_ROW;
        ParticleInterval INTERVAL_FIRST_COLUMN;
        ParticleInterval INTERVAL_FIRST_STACK;
        ParticleInterval INTERVAL_LAST_ROW;
    
    	Lock lock_;
    
        //Applied during update
        glm::vec3 externalAcceleration;
        glm::vec3 externalForce;

        friend void testMCS();

    std::vector<CollisionPlane> collisionPlanes;

    private:
        void initParticles();
        void initConnections();
        void initTriangles();
        void initVertices();
        void initParticleIntervals();

    
    
        // The dimensions constants for the MCS
        const int N_ROWS;
        const int N_COLS;
        const int N_STACKS;
};
#endif