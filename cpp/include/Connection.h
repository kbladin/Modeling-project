#ifndef CONNECTION_H
#define CONNECTION_H

#include <glm/glm.hpp>
#include "Particle.h"

class Connection{
public:
	Connection(Particle * p1 = NULL, 
			   Particle * p2 = NULL, 
			   float length = 1, 
			   float k = 1,
			   float b = 1);

	void setConnectionLength(float l);
	void setSpringConstant(float k);
	void setDamperConstant(float b);

	
	//The particle p1 will be affected by a force f which
	//is calculated by calcForce(float). The particle p2
	//will be affected by the force -f. This method applies
	//these forces to the p1 and p2, respectively.
	void applyForcesToConnectedParticles(float dt);

protected:
	
	//This method calculates the force which this
	//connection affects the particle p1 with.
	glm::vec3 calcForce(float dt) const;

private:
	friend void testConnection();

	glm::vec3 getDeltaPosition() const;
	glm::vec3 getDeltaVelocity() const;

	Particle * _p1;
	Particle * _p2;

	float _l;
	float _k;
	float _b;
};

#endif
