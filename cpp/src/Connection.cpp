#include "Connection.h"

Connection::Connection(Particle * p1, Particle * p2, float length, float k,float b):
_p1(p1), _p2(p2), _l(length), _k(k), _b(b){ }

void Connection::setConnectionLength(float l){
	if (0.0f <= l)
		_l = l;
}

void Connection::setSpringConstant(float k){
	if (0.0f <= k)
		_k = k;
}

void Connection::setDamperConstant(float b){
	if (0.0f <= b)
		_b = b;
}


void Connection::applyForcesToConnectedParticles(float dt){
	glm::vec3 force = calcForce();

	/*
	//RK4
	glm::vec3 a0 = getDeltaVelocity();
	glm::vec3 a1 = calcForce();

	glm::vec3 b0 = (dt/2) * (a1/_p1->getMass() + a1/_p2->getMass());
	glm::vec3 b1 = calcForce((dt/2)*a0, (dt/2)*a1);

	glm::vec3 c0 = (dt/2) * (b1/_p1->getMass() + b1/_p2->getMass());
	glm::vec3 c1 = calcForce((dt/2)*b0, (dt/2)*b1);

	glm::vec3 d0 = dt * (c1/_p1->getMass() + c1/_p2->getMass());
	glm::vec3 d1 = calcForce(dt*c0, dt*c1);

	glm::vec3 force = (1/6.0f)*(a1 + 2.0f*b1 + 2.0f*c1 + d1);
	*/

	_p1->storeForce(force);
	_p2->storeForce(-force);
}

glm::vec3 Connection::calcForce(const glm::vec3& delta_p_offset, const glm::vec3& delta_v_offset) const{
	glm::vec3 delta_p = getDeltaPosition() + delta_p_offset;
	glm::vec3 delta_v = getDeltaVelocity() + delta_v_offset;
	glm::vec3 delta_p_hat = glm::normalize(delta_p);
	float spring_elongation = glm::length(delta_p) - _l;
	return (-_k*spring_elongation - _b*glm::dot(delta_v,delta_p_hat))*delta_p_hat;
}

glm::vec3 Connection::getDeltaPosition() const{
	return _p1->readPosition() - _p2->readPosition();
}

glm::vec3 Connection::getDeltaVelocity() const{
	return _p1->readVelocity() - _p2->readVelocity();
}