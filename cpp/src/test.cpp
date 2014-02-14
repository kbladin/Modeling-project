#include <iostream>
#include <cassert>
#include "Particle.h"

static void testParticle(){
	Particle p(2.0f, glm::vec3(1,2,3), glm::vec3(5,6,7));
    assert(p.readPosition() == glm::vec3(1,2,3));
    assert(p.readVelocity() == glm::vec3(5,6,7));
    Particle::swapBuffers();
    assert(p.readPosition() == glm::vec3(0,0,0));
    assert(p.readVelocity() == glm::vec3(0,0,0));
    
    p.writePosition(glm::vec3(2,2,2));
    p.writeVelocity(glm::vec3(3,3,3));
    assert(p.readPosition() != glm::vec3(2,2,2));
    assert(p.readVelocity() != glm::vec3(3,3,3));
    Particle::swapBuffers();
    assert(p.readPosition() == glm::vec3(2,2,2));
    assert(p.readVelocity() == glm::vec3(3,3,3));
}