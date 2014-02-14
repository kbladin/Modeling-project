#include <iostream>
#include <glm/glm.hpp>
//#include <models.h>
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

        /*
        float * masses[N_MASSES];
        glm::vec2 * positions[N_MASSES][2];
        glm::vec2 * velocities[N_MASSES][2];
        glm::vec2 * forces[N_MASSES];

        int * connected_masses[N_CONNECTIONS][2];
        */

        float * masses;
        glm::vec2 * positions;
        glm::vec2 * velocities;
        glm::vec2 * forces;

        int * connected_masses[N_CONNECTIONS][2];

};

//Constructor
MCS::MCS(int n_rows, int n_cols) : N_ROWS(n_rows), N_COLS(n_cols){
//	const int N_ROWS = n_rows;
//	const int N_COLS = n_cols;

	N_TYPE1 = (N_ROWS-1)*N_COLS; // |
	N_TYPE2 = (N_ROWS-1)*(N_COLS-1); // /
	N_TYPE3 = N_ROWS*(N_COLS-1); // _
	N_TYPE4 = N_TYPE2; // \

	N_MASSES = N_ROWS*N_COLS;
	N_CONNECTIONS = N_TYPE1+N_TYPE2+N_TYPE3+N_TYPE4;

	setStartingValues();

	// Calculate connections
    for (int i = 0; i < N_CONNECTIONS; ++i){
        connection2massIndices(i, connected_masses[i][0], connected_masses[i][1], N_ROWS, N_COLS);
    }

}

//Destructor
MCS::~MCS(){
	delete[] connected_masses;
	delete[] masses;
	delete[] positions;
	delete[] velocities;
	delete[] forces;
}

void MCS::setStartingValues(){
// Set starting values to masses, positions velocities and forces
    for (int i = 0; i < N_MASSES; ++i)
    {
        masses[i] = 1.0f;
        int row = i%N_COLS;
        int col = floor(i/N_COLS);
        positions[i][0] = glm::vec2(row,col);		// Read buffer postition = row, col for each mass
        positions[i][1] = glm::vec2(0,0);			// Write buffer position = 0,0
        velocities[i][0] = glm::vec2(0,0);
        velocities[i][1] = glm::vec2(0,0);
        forces[i] = glm::vec2(0,0);
    }
    positions[0][0] = glm::vec2(0.0f,-1.0f);
    velocities[0][0] = glm::vec2(0.0f,-100.0f);

}
/*
//constructor
MSDS::MSDS(float spring_const, float damper_const, float spring_l, n_rows, n_cols) : MCS(n_rows, n_cols);{
	// Set values for springs, dampers, and lengths
    for (int i = 0; i < N_CONNECTIONS; ++i)
    {
        spring_constants[i] = spring_const;
        damper_constants[i] = damper_const;
        spring_lengths[i] = spring_l;
    }

    // Set type_2 connections to legnth sqrt(2)
    for (int i = N_TYPE1; i < N_TYPE1+N_TYPE2; ++i)
    {
        spring_lengths[i] = sqrt(2);
    }

    // Set type_4 connections to legnth sqrt(2)
    for (int i = N_TYPE1+N_TYPE2+N_TYPE3; i < N_CONNECTIONS; ++i)
    {
        spring_lengths[i] = sqrt(2);
    }
}

//destructor
MSDS::~MSDS(){
	delete[] spring_constants;
	delete[] damper_constants;
	delete[] spring_lengths;
}
*/
int main(){
    std::cout << "Hej" << std::endl;
}


