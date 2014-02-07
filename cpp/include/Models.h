#include <GL/glm.h>

/** Classes **/
// The abstract base class for all systems 
class Model{
	public:
		float diffEq();
};

// The abstract subclass MCS - Mass Connection System
class MCS : public Model{
	public:
		// Constructor
		MCS(int numberOfMasses, int numberOfConnections){
			// Allocate memory for variable length arrays
			masses = new float [numberOfMasses];
			connections = new float [numberOfConnections];
			positions = new glm::vec3 [numberOfMasses];
			velocities = new glm::vec3 [numberOfMasses];
			forces = new glm::vec3 [numberOfMasses];
		}
		// Destructor
		~MCS(){
			delete[] masses;
			delete[] connections;
			delete[] positions;
			delete[] velocities;
			delete[] forces;
		}
	protected:
		//int numberOfMasses;
		float * masses;
		float * connections;
		glm::vec3 * positions;
		glm::vec3 * velocities;
		glm::vec3 * forces;
};

// The subclass MSDS - Mass Spring Damper System
class MSDS : public MCS{
	public:
		// Constructor
		MSDS(int numberOfMasses, int numberOfConnections) : MCS(numberOfMasses, numberOfConnections){
			springDampers = new SpringDamper [numberOfConnections];
		}
		// Destructor
		~MSDS(){
			delete[] springDampers;
		}

	protected:
		SpringDamper * springDampers;
};

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