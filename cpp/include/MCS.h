#ifndef MCS_H
#define MCS_H

#include <vector>
#include <glm/glm.hpp>
#include "connection2massindices.h"
#include "debugtools.h"


/** Classes **/
struct IndexedTriangle{
    int idx1;
    int idx2;
    int idx3;
};

struct Particles{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> velocities;
    std::vector<glm::vec3> accelerations;
    std::vector<glm::vec3> forces;
    std::vector<float> masses;

    static enum Select{
        ALL = 0,
        OUTSIDES,
        EDGES,
        CORNERS
    } select;

    void setMasses(float m, int select = 0){
        for (int i = 0; i < masses.size(); ++i){
            masses[i] = m;
        }
    }
};

struct Connections{
    std::vector<float> lengths;
    std::vector<float> springConstants;
    std::vector<float> damperConstants;
    std::vector<int> particle1;
    std::vector<int> particle2;

    //The number of connections of each type/direction
    int nType[13];
    int startType[13];

    static enum Select{
        ALL = 0,
        ALONG_ROWS,
        ALONG_COLS,
        ALONG_STACKS
    } select;

    void setLengths(float l, int select = 0){
        for (int i = 0; i < startType[3]; ++i)
            lengths[i] = l;
        for (int i = startType[3]; i < startType[9]; ++i)
            lengths[i] = l*sqrt(2.0f);
        for (int i = startType[9]; i < lengths.size(); ++i)
            lengths[i] = l*sqrt(3);
    }

    void setSpringConstant(float s, int select = 0){
        for (int i = 0; i < springConstants.size(); ++i){
            springConstants[i] = s;
        }
    }

    void setDamperConstant(float d, int select = 0){
        for (int i = 0; i < damperConstants.size(); ++i){
            damperConstants[i] = d;
        }
    }
};

struct CollisionPlane{
    glm::vec3 normal;
    float position;
    float elasticity;
    float friction;
};

struct Triangles{
    std::vector<IndexedTriangle> triangleIndices;
    std::vector<glm::vec3> normals;
};

struct NewTriangles{
    std::vector<IndexedTriangle> triangleIndices;
    std::vector<glm::vec3> normals;
};

struct Vertices{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec2> UVs;
    std::vector<int> particleIndices;
};

// The MCS - Mass Connection System
class MCS{

    public:
        // Constructor
        MCS(const int n_rows, const int n_cols, const int n_stacks);

        void rotate(glm::vec3 axisOfRotation, float amount);
        void addRotation(glm::vec3 axisOfRotation, float amount);
        void setAvgPosition(glm::vec3 pos);
        void setAvgVelocity(glm::vec3 pos);

        void updateNormals();
        void updateVertexPositions();


        glm::vec3 centerOfMass() const;
        glm::vec3 averagePosition() const;
        glm::vec3 averageVelocity() const;

        //Get-functions to access the protected variables in the MCS class
        int getNumberOfParticles() const;
        int getNumberOfConnections() const;
        int getNumberOfVertices() const;


        Particles particles;
        Connections connections;
        Vertices vertices;
        Triangles triangles;
        NewTriangles newTriangles;

        void addCollisionPlane(glm::vec3 normal, 
                               float position, 
                               float elasticity = 0.0f,
                               float friction = 0.0f);
        
        //The dimensions constants for the MCS
        const int N_ROWS;
        const int N_COLS;
        const int N_STACKS;

        //Applied during update
        glm::vec3 externalAcceleration;
        glm::vec3 externalForce;

        void calcConnectionForcesOnParticles();
        void calcConnectionForcesOnParticles(
            const std::vector<glm::vec3>& delta_v_offset,
            const std::vector<glm::vec3>& delta_p_offset);
        void calcAccelerationOfParticle(int i);
        void checkCollisions(glm::vec3& pos, glm::vec3& vel) const;

        friend void testMCS();

    private:

        void initParticles();
        void initConnections();
        void initTriangles();
        void initVertices();
        
        void triangle2particleIndices(int triangleIndex, int &particleIndex1, int &particleIndex2, int &particleIndex3);
        void vertex2particleIndex(int vertexIndex, int &particleIndex);
        void triangle2vertexIndices(int triangleIndex, int &vertexIndex1, int &vertexIndex2, int &vertexIndex3);


        std::vector<CollisionPlane> collisionPlanes;
};
#endif