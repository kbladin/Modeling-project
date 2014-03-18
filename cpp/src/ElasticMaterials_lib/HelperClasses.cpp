#include "ElasticMaterials_lib/HelperClasses.h"

CollisionPlane::CollisionPlane(glm::vec3 normal, float position, float elasticity, float friction) :
normal_(glm::normalize(normal)), position_(position), elasticity_(elasticity), friction_(friction) {
    
    const int N_VERTS = 8; // Both sides
    float scale = 200.0f;
    glm::vec3 axis1;
    glm::vec3 axis2;
    
    if ( glm::dot(normal_, glm::vec3(1.0f,0.0f,0.0f)) != glm::length(normal_))
        axis1 = glm::normalize(glm::cross(normal_, glm::vec3(1.0f,0.0f,0.0f)));
    else
        axis1 = glm::normalize(glm::cross(normal_, glm::vec3(0.0f,1.0f,0.0f)));
    axis2 = glm::cross(axis1, normal_);
    
    vertices_.positions = std::vector<glm::vec3>(N_VERTS);
    vertices_.normals = std::vector<glm::vec3>(N_VERTS);
    vertices_.colors = std::vector<glm::vec3>(N_VERTS);
    vertices_.UVs = std::vector<glm::vec2>(N_VERTS);
    
    triangles_.triangleIndices = std::vector<IndexedTriangle>(4);
    
    // Positions
    for (int i=0; i<2; ++i) {
        vertices_.positions[0+i*4] = (-0.5f * axis1 - 0.5f * axis2) * scale + normal_*position_;
        vertices_.positions[1+i*4] = (-0.5f * axis1 + 0.5f * axis2) * scale + normal_*position_;
        vertices_.positions[2+i*4] = (0.5f * axis1 - 0.5f * axis2) * scale + normal_*position_;
        vertices_.positions[3+i*4] = (0.5f * axis1 + 0.5f * axis2) * scale + normal_*position_;
    }
    // Normals
    for (int i=0; i<4; ++i) {
        vertices_.normals[i] = normal_;
        vertices_.normals[i+4] = -normal_;
    }
    // Colors
    for (int i=0; i<N_VERTS; ++i) {
        vertices_.colors[i] = glm::vec3(float(rand())/RAND_MAX, float(rand())/RAND_MAX, float(rand())/RAND_MAX);
    }
    // UVs ...
    
    //Triangles
    triangles_.triangleIndices[0].idx1 = 0;
    triangles_.triangleIndices[0].idx2 = 2;
    triangles_.triangleIndices[0].idx3 = 3;
    triangles_.triangleIndices[1].idx1 = 0;
    triangles_.triangleIndices[1].idx2 = 3;
    triangles_.triangleIndices[1].idx3 = 1;
    
    triangles_.triangleIndices[2].idx1 = 3;
    triangles_.triangleIndices[2].idx2 = 2;
    triangles_.triangleIndices[2].idx3 = 0;
    triangles_.triangleIndices[3].idx1 = 1;
    triangles_.triangleIndices[3].idx2 = 3;
    triangles_.triangleIndices[3].idx3 = 0;
}

Lock::Lock(){
    ParticleInterval pi;
    pi.start = 0;
    pi.skip = 1;
    pi.end = 0;
    pi_ = pi;
    locked = false;
}
Lock::Lock(ParticleInterval pi){
    pi_ = pi;
    locked = true;
}
ParticleInterval Lock::getParticleInterval(){
    return pi_;
}
bool Lock::particleLocked(int particle_index){
    return locked &&
    particle_index > pi_.start &&
    particle_index < pi_.end &&
    particle_index % pi_.skip == 0;
}
