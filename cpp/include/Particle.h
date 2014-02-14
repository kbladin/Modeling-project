#include <glm/glm.hpp>

#ifndef PARTICLE_H
#define PARTICLE_H

class Particle
{
public:
	Particle(const float mass = 1, 
			 const glm::vec3& position = glm::vec3(0,0,0), 
			 const glm::vec3& velocity = glm::vec3(0,0,0));
	~Particle();
	
	const glm::vec3& readPosition() const;
	const glm::vec3& readVelocity() const;
	const glm::vec3& readForce() const;

	void writePosition(const glm::vec3& position);
	void writeVelocity(const glm::vec3& velocity);
	void storeForce(const glm::vec3& force);

	void resetForce();

	static void swapBuffers();

private:

	float _mass;
	glm::vec3 _position[2];
	glm::vec3 _velocity[2];
	glm::vec3 _force;

	static int read_buffer_index;
	static int write_buffer_index;

};

#endif 
