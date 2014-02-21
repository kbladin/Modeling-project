#include "Particle.h"

Particle::Particle(const float mass, const glm::vec3& position, const glm::vec3& velocity){
	_mass = mass;
	_position = position;
	_velocity = velocity;
	_force = glm::vec3(0,0,0);
}

const glm::vec3& Particle::readPosition() const{
	return _position;
}

const glm::vec3& Particle::readVelocity() const{
	return _velocity;
}

const glm::vec3& Particle::readForce() const{
	return _force;
}

const float Particle::getMass() const{
	return _mass;
}


void Particle::writePosition(const glm::vec3& position){
	_position = position;
}

void Particle::writeVelocity(const glm::vec3& velocity){
	_velocity = velocity;
}

void Particle::storeForce(const glm::vec3& force){
	_force += force;
}

void Particle::applyForce(float dt){
	glm::vec3 a = _force/_mass - glm::vec3(0.0f, 1.0f, 0.0f)*9.81f;
	glm::vec3 v = _velocity + a*dt;
	glm::vec3 p = _position + v*dt;

	if (p[1] < -10.0f){
		p[1] = -10.0f;
		v[1] = -v[1];

		v[0] *= 0.9f;
		v[2] *= 0.9f;
	}

	_velocity = v;
	_position = p;

	_force = glm::vec3(0,0,0);
}