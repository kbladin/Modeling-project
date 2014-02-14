
#ifndef MODELS_H
#define MODELS_H

/** Classes **/
// The abstract base class for all systems 
/*class Model{
	public:
		float diffEq();
};*/

// The abstract subclass MCS - Mass Connection System
class MCS /*: public Model*/{
	public:
		// Constructor
		MCS(int n_rows, int n_cols);
		// Destructor
		~MCS();

	protected:
		const int N_ROWS;
		const int N_COLS;

		const int N_TYPE1;
		const int N_TYPE2;
		const int N_TYPE3;
		const int N_TYPE4;

		const int N_MASSES;
		const int N_CONNECTIONS;

		float masses[N_MASSES];
		glm::vec2 positions[N_MASSES][2];
		glm::vec2 velocities[N_MASSES][2];
		glm::vec2 forces[N_MASSES];

		int connected_masses[N_CONNECTIONS][2];

};

// The subclass MSDS - Mass Spring Damper System
class MSDS : public MCS{
	public:
		// Constructor
		MSDS(float spring_const, float damper_const, float spring_l, int n_rows, int n_cols);
		// Destructor
		~MSDS();

	protected:
		//SpringDamper * springDampers;
		float spring_constants[N_CONNECTIONS];
		float damper_constants[N_CONNECTIONS];
		float spring_lengths[N_CONNECTIONS];
};
/*
// The subclass MSDFS - Mass Spring Damper Friction System
class MSDFS : public MSDS{
	public:
		// Constructor
		MSDFS(int numberOfMasses, int numberOfConnections) : MSDS(numberOfMasses, numberOfConnections){
			friction = new Friction [numberOfConnections];
		}
		// Destructor
		~MSDFS(){
			delete[] friction;
		}
		Friction * friction;
};
*/
#endif