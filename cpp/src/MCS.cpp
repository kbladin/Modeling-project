#include "MCS.h"


//Constructor
MCS::MCS(const int n_rows, const int n_cols, const int n_stacks):   
    N_ROWS(n_rows),N_COLS(n_cols),N_STACKS(n_stacks)
{
	initParticles();
    initConnections();
}

void MCS::initParticles(){

    int numberOfParticles = N_COLS*N_ROWS*N_STACKS;

    particles.positions = std::vector<glm::vec3>(numberOfParticles);
    particles.velocities = std::vector<glm::vec3>(numberOfParticles);
    particles.accelerations = std::vector<glm::vec3>(numberOfParticles);
    particles.forces = std::vector<glm::vec3>(numberOfParticles);
    particles.masses = std::vector<float>(numberOfParticles);

    for (int i = 0; i < numberOfParticles; ++i){
        float x = i%N_COLS;
        float y = (i/N_COLS)%N_ROWS;
        float z = i/(N_COLS*N_ROWS);

        particles.positions[i] = glm::vec3(x,y,z);
        particles.velocities[i]  = glm::vec3(0.f,0.f,0.f);
        particles.accelerations[i] = glm::vec3(0.f,0.f,0.f);
        particles.forces[i] = glm::vec3(0.f,0.f,0.f);
        particles.masses[i] = 1.0f;    
    }
}

void MCS::initConnections(){
    numberOfConnectionsOfType[0] = N_ROWS*(N_COLS-1)*N_STACKS;
    numberOfConnectionsOfType[1] = (N_ROWS-1)*N_COLS*N_STACKS;
    numberOfConnectionsOfType[2] = N_ROWS*N_COLS*(N_STACKS-1);
    numberOfConnectionsOfType[3] = (N_ROWS-1)*(N_COLS-1)*N_STACKS;
    numberOfConnectionsOfType[4] = (N_ROWS-1)*(N_COLS-1)*N_STACKS;
    numberOfConnectionsOfType[5] = N_ROWS*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[6] = N_ROWS*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[7] = (N_ROWS-1)*N_COLS*(N_STACKS-1);
    numberOfConnectionsOfType[8] = (N_ROWS-1)*N_COLS*(N_STACKS-1);
    numberOfConnectionsOfType[9] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[10] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[11] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[12] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);

    startOfConnectionOfType[0] = 0;
    int numberOfConnections = numberOfConnectionsOfType[0];
    for (int i = 1; i < 13; ++i){
        startOfConnectionOfType[i] = startOfConnectionOfType[i-1] + numberOfConnectionsOfType[i-1];
        numberOfConnections += numberOfConnectionsOfType[i];
    }


    connections.lengths = std::vector<float>(numberOfConnections);
    connections.springConstants = std::vector<float>(numberOfConnections);
    connections.damperConstants = std::vector<float>(numberOfConnections);
    connections.particle1 = std::vector<int>(numberOfConnections);
    connections.particle2 = std::vector<int>(numberOfConnections);

    // Calculate connections
    int p_index1;
    int p_index2;
    for (int i = 0; i < numberOfConnections; ++i){
        connection2massIndices3D(i, p_index1, p_index2, N_ROWS, N_COLS, N_STACKS);
        
        connections.lengths[i] = 1.0f;
        connections.springConstants[i] = 500.0f;
        connections.damperConstants[i] = 5.0f;
        connections.particle1[i] = p_index1;
        connections.particle2[i] = p_index2;
    }

    //Set length of 2D-diagonal springs to sqrt(2)
    for (int i = startOfConnectionOfType[3]; i < startOfConnectionOfType[9]; ++i){
        connections.lengths[i] = sqrt(2.0f);
    }

    //Set length of 3D-diagonal springs to sqrt(3)
    for (int i = startOfConnectionOfType[9]; i < numberOfConnections; ++i){
        connections.lengths[i] = sqrt(3.0f);
    }
}

void MCS::addCollisionPlane(glm::vec3 normal, float position, float elasticity, float friction){
    CollisionPlane cp; 
    cp.normal = glm::normalize(normal);
    cp.position = position;
    collisionPlanes.push_back(cp);
}

void MCS::update(float dt, glm::vec3 externalAcceleration){

    calcConnectionForcesOnParticles();
    calcAccelerationOfParticles(glm::vec3(0,-1,0)*9.82f);
    updateParticles(dt);

}

void MCS::calcConnectionForcesOnParticles(glm::vec3 delta_p_offset, glm::vec3 delta_v_offset){
    glm::vec3 delta_p;
    glm::vec3 delta_v;
    glm::vec3 delta_p_hat;
    glm::vec3 force;

    float k;
    float l;
    float b;
    float spring_elongation;

    // Calculate the forces of the springs
    for (int i = 0; i < getNumberOfConnections(); ++i){
        k = connections.springConstants[i];
        l = connections.lengths[i];
        b = connections.damperConstants[i];

        delta_p = particles.positions[connections.particle1[i]] - particles.positions[connections.particle2[i]]; 
        delta_p += delta_p_offset;
        delta_p_hat = glm::normalize(delta_p);

        delta_v = particles.velocities[connections.particle1[i]] - particles.velocities[connections.particle2[i]]; 
        delta_v += delta_v_offset;
        

        spring_elongation = glm::length(delta_p) - l;
        //float sign = spring_elongation >= 0.0f ? 1.0f : -1.0f;

        force = (-k*spring_elongation - b*glm::dot(delta_v,delta_p_hat))*delta_p_hat;
        particles.forces[connections.particle1[i]] += force;
        particles.forces[connections.particle2[i]] -= force;
    }
}

void MCS::calcAccelerationOfParticles(glm::vec3 externalAcceleration, glm::vec3 externalForce){
    for (int i = 0; i < getNumberOfParticles(); ++i){
        particles.accelerations[i] = 
            (particles.forces[i] + externalForce)/particles.masses[i] + 
            externalAcceleration;

        //Reset forces
        particles.forces[i][0] = 0.0f;
        particles.forces[i][1] = 0.0f;
        particles.forces[i][2] = 0.0f;
    }
}

void MCS::updateParticles(float dt){
    //Allocate memory for two new position and velocity
    glm::vec3 new_position;
    glm::vec3 new_velocity;

    for (int i = 0; i < getNumberOfParticles(); ++i){
        //Calc new position and velocity
        new_velocity = particles.velocities[i] + particles.accelerations[i] * dt;
        new_position = particles.positions[i] + new_velocity * dt;

        //Check collisions with collision planes
        checkCollisions(new_position, new_velocity);

        //Set new position and velocity
        particles.velocities[i] = new_velocity;
        particles.positions[i] = new_position;
    }
}

void MCS::checkCollisions(glm::vec3& p, glm::vec3& v) const{
    glm::vec3 n;
    float pos;
    for (int i = 0; i < collisionPlanes.size(); ++i){
        n = collisionPlanes[i].normal;
        pos = collisionPlanes[i].position;
        float p_dot_n = glm::dot(p,n);

        if (p_dot_n < pos){
            glm::vec3 p_offset = (p_dot_n - pos)*n;
            glm::vec3 v_parallel_n = glm::dot(v,n)*n;
            glm::vec3 v_orthogonal_n = v - v_parallel_n;
            //std::cout << "--" << std::endl;
            //std::cout << "           v: " << v[0] << " " << v[1] << " " << v[2] << std::endl;
            //std::cout << "v_parallel_n: "<< v_parallel_n[0] << " " << v_parallel_n[1] << " " << v_parallel_n[2] << std::endl;

            p -= p_offset;
            v -= v_parallel_n*(1.0f+collisionPlanes[i].elasticity);
            v -= v_orthogonal_n*collisionPlanes[i].friction;
        }
    }
}

void MCS::rotate(glm::vec3 axisOfRotation, float degrees){
    for (int i = 0; i < getNumberOfParticles(); ++i){
        //Cant rotate with glm::rotate???
    }
}

void MCS::addRotation(glm::vec3 axisOfRotation, float amount){
    glm::vec3 avgVelBefore = averageVelocity();

    glm::vec3 mid = centerOfMass();

    for (int i = 0; i < getNumberOfParticles(); ++i){
        glm::vec3 midToParticle = -particles.positions[i] + mid;
        glm::vec3 rotDir = glm::cross(midToParticle, glm::normalize(axisOfRotation));
        particles.velocities[i] += rotDir*amount;
    }

    glm::vec3 avgVelAfter = averageVelocity();
    glm::vec3 avgVelError = avgVelAfter - avgVelBefore;
    for (int i = 0; i < getNumberOfParticles(); ++i){
        particles.velocities[i] -= avgVelError;
    }
}

void MCS::setAvgPosition(glm::vec3 pos){
    glm::vec3 avgPos = averagePosition();
    glm::vec3 toAdd = pos - avgPos;
    for (int i = 0; i < getNumberOfParticles(); ++i){
        particles.positions[i] += toAdd;
    }
}

void MCS::setAvgVelocity(glm::vec3 vel){
    glm::vec3 avgVel = averageVelocity();
    glm::vec3 toAdd = vel - avgVel;
    for (int i = 0; i < getNumberOfParticles(); ++i){
        particles.velocities[i] += toAdd;
    }
}

glm::vec3 MCS::centerOfMass() const{
    glm::vec3 weightedSum(0,0,0);
    float totalMass;
    for (int i = 0; i < getNumberOfParticles(); ++i){
        weightedSum += particles.positions[i] * particles.masses[i];
        totalMass += particles.masses[i];
    }
    return weightedSum/totalMass;
}

glm::vec3 MCS::averagePosition() const{
    glm::vec3 sum(0,0,0);
    for (int i = 0; i < getNumberOfParticles(); ++i){
        sum += particles.positions[i];
    }
    return sum/(float)getNumberOfParticles();
}

glm::vec3 MCS::averageVelocity() const{
    glm::vec3 sum(0,0,0);
    for (int i = 0; i < getNumberOfParticles(); ++i){
        sum += particles.velocities[i];
    }
    return sum/(float)getNumberOfParticles();
}

int MCS::getNumberOfParticles() const{
    return particles.positions.size();
}

int MCS::getNumberOfConnections() const{
    return connections.lengths.size();
}
