#ifndef HELPER_CLASSES_H
#define HELPER_CLASSES_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "debugtools.h"
#include "conversions.h"

//STRUCTS

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

struct Triangles{
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

struct ParticleInterval{
    int start;
    int end;
    int skip;
};

//CLASSES

class CollisionPlane{
public:
    CollisionPlane(glm::vec3 normal, float position, float elasticity, float friction);
    
    const glm::vec3 normal_;
    const float position_;
    const float elasticity_;
    const float friction_;
    
    Vertices vertices_;
    Triangles triangles_;
};

class Lock{
public:
    Lock();
    Lock(ParticleInterval pi);
    bool locked;
    ParticleInterval getParticleInterval();
    bool particleLocked(int particle_index);
private:
    ParticleInterval pi_;
};


#endif