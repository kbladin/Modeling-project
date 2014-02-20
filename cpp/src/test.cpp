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
    std::stringstream ss;

    ss << "testing Connection ..." << std::endl;
    Particle p1(1.0f, glm::vec3(5,1,0), glm::vec3(0,0,0));
    Particle p2(1.0f, glm::vec3(0,0,0), glm::vec3(0,0,0));
    Connection c(&p1,&p2,1,10,5);
    assert(c._l == 1);
    assert(c._k == 10);
    assert(c._b == 5);

    //assert(calcForce(0.0f) == glm::vec3(10,0,0))
    ss << "  Current force on p1 from connection:" << std::endl;
    ss << "  c.calcForce() = " << c.calcForce() << std::endl;

    ss << "\n";
    ss << "  Calculate forces on connected particles with RK4:" << std::endl;
    float dt = 0.05f;
    c.applyForcesToConnectedParticles(dt);
    ss << "  p1._force = " << p1._force << std::endl;
    ss << "  p2._force = " << p2._force << std::endl;
    ss << "  p1 and p2 applyForce() " << std::endl;
    p1.applyForce(dt);
    p2.applyForce(dt);

    ss << "  Particle::swapBuffers() " << std::endl;
    Particle::swapBuffers();

    ss << "\n";
    ss << "  p1.readPosition() = " << p1.readPosition() << std::endl;
    ss << "  p2.readPosition() = " << p2.readPosition() << std::endl;
    ss << "Done!" << std::endl;

    //std::cout << ss.str();
}

//This test function should implement some
//nice test cases when MSC-class is done 
void testMCS(){
    std::cout << "Testing MCS" << std::endl;
    MCS mcs(1,2,2);
    assert(mcs.getNumberOfParticles() == 1*2*2);
    // uncomment this when class is implemented
    //assert(6 == mcs.numberOfConnections()); 
}