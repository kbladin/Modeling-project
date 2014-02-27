#ifndef NUMERICAL_METHODS_H
#define NUMERICAL_METHODS_H

#include "MCS.h"


/** Declare global functions  **/
glm::vec3 calculateEulerExplicit();
glm::vec3 calculateRungeKutta();

class NumericalMethod{
	public:
		virtual void update(MCS& mcs, float dt) = 0;
};

class EulerExplicit: public NumericalMethod{
	public:
		void update(MCS& mcs, float dt);
};

class RungeKutta: public NumericalMethod{
	public:
		RungeKutta(std::vector<float> weights);
		void setWeights(std::vector<float> weights);
		void update(MCS& mcs, float dt);

	private:
		void allocMemoryFor(const MCS& mcs);

		std::vector<float> w;
		float wSum;

		std::vector<glm::vec3> delta_v_offsets;
	    std::vector<glm::vec3> delta_p_offsets;

	    std::vector<std::vector<glm::vec3> > ka;
	    std::vector<std::vector<glm::vec3> > kv;
};
#endif