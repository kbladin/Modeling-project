#include "test.h"

static std::ostream& operator<<(std::ostream& os, const glm::vec3& vec){
    return os << "(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
}

void testParticle(){
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


void testConnection(){
    Particle p1(2.0f, glm::vec3(1,2,3), glm::vec3(5,6,7));
    Particle p2(1.0f, glm::vec3(0,0,0), glm::vec3(0,0,0));
    Connection c(p1,p2,1,10,5);
    assert(c._l == 1);
    assert(c._k == 10);
    assert(c._b == 5);

    //std::cout << c.calcForce(0.0f) << std::endl;

    p1.writePosition(glm::vec3(0,0,0));
    p2.writePosition(glm::vec3(2,0,0));
    Particle::swapBuffers();
    assert(calcForce(0.0f) == glm::vec3(10,0,0))
    //std::cout << c.calcForce(0.0f) << std::endl;
}
