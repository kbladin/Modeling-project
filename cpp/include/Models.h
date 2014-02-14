
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
        MCS(const int n_rows, const int n_cols, const int n_stacks);
        // Destructor
        ~MCS();

        //Get-functions to access the protected variables in the MCS class
        int getRows();
        int getCols();
        int getStack();
        int getParticles();
        int getConnections();
        

    protected:
        //The dimensions variables for the MCS
        const int N_ROWS;
        const int N_COLS;
        const int N_STACKS;

        //The number of connections of each type/direction
        const int N_TYPE1;
        const int N_TYPE2;
        const int N_TYPE3;
        const int N_TYPE4;

        //The total number of particles and connections
        const int N_PARTICLES;
        const int N_CONNECTIONS;

        void setStartingValues();
        
        // Pointers for dynamically allocated arrays of particle and connetion objects
        Particle * particles;
        Connection * connections;

};

// The subclass MSDS - Mass Spring Damper System
/*
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
};*/
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