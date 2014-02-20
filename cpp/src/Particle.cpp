#include "Particle.h"

// Static variables
//IS THIS FUNCTIONALITY EVEN NEEDED ANYMORE?
int Particle::read_buffer_index = 0;
int Particle::write_buffer_index = 0;

// Static function
void Particle::swapBuffers(){
	return;
	write_buffer_index = read_buffer_index;
	read_buffer_index = (read_buffer_index+1)%2;
}


Particle::Particle(const float mass, const glm::vec3& position, const glm::vec3& velocity){
	_mass = mass;
	_position[read_buffer_index] = position;
	_velocity[read_buffer_index] = velocity;
	_force = glm::vec3(0,0,0);
}

const glm::vec3& Particle::readPosition() const{
	return _position[read_buffer_index];
}

const glm::vec3& Particle::readVelocity() const{
	return _velocity[read_buffer_index];
}

const glm::vec3& Particle::readForce() const{
	return _force;
}

const float Particle::getMass() const{
	return _mass;
}


void Particle::writePosition(const glm::vec3& position){
	_position[write_buffer_index] = position;
}

void Particle::writeVelocity(const glm::vec3& velocity){
	_velocity[write_buffer_index] = velocity;
}

void Particle::storeForce(const glm::vec3& force){
	_force += force;
}

void Particle::applyForce(float dt){
	glm::vec3 a = _force/_mass - glm::vec3(0.0f, 1.0f, 0.0f)*9.81f;
	glm::vec3 v = _velocity[read_buffer_index] + a*dt;
	glm::vec3 p = _position[read_buffer_index] + v*dt;

	if (p[1] < -10.0f){
		p[1] = -10.0f;
		v[1] = -v[1];

		v[0] *= 0.9f;
		v[2] *= 0.9f;
	}

	_velocity[write_buffer_index] = v;
	_position[write_buffer_index] = p;

	_force = glm::vec3(0,0,0);
}