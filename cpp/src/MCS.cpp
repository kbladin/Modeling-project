#include "MCS.h"


//Constructor
MCS::MCS(const int n_rows, const int n_cols, const int n_stacks):   
    N_ROWS(n_rows),N_COLS(n_cols),N_STACKS(n_stacks)
    //particles(n_rows*n_cols*n_stacks)
{
	initParticles();
    initConnections();
}


void MCS::initParticles(){
    
    for (int i = 0; i < N_COLS*N_ROWS*N_STACKS; ++i){
        float x = i%N_COLS;
        float y = (i/N_COLS)%N_ROWS;
        float z = i/(N_COLS*N_ROWS);
        particlePositions.push_back(glm::vec3(x,y,z));
        particleVelocities.push_back(glm::vec3(0.f,0.f,0.f)); 
        particleAccelerations.push_back(glm::vec3(0.f,0.f,0.f));
        particleMasses.push_back(glm::vec3(1.f,1.f,1.f));
    }
}

void MCS::initConnections(){
    numberOfConnectionsOfType[0] = N_ROWS*(N_COLS-1)*N_STACKS;
    numberOfConnectionsOfType[1] = (N_ROWS-1)*N_COLS*N_STACKS;
    numberOfConnectionsOfType[2] = N_ROWS*N_COLS*(N_STACKS-1);
    numberOfConnectionsOfType[3] = (N_ROWS-1)*(N_COLS-1)*N_STACKS;
    numberOfConnectionsOfType[4] = (N_ROWS-1)*(N_COLS-1)*N_STACKS;
    numberOfConnectionsOfType[5] = N_ROWS*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[6] = N_ROWS*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[7] = (N_ROWS-1)*N_COLS*(N_STACKS-1);
    numberOfConnectionsOfType[8] = (N_ROWS-1)*N_COLS*(N_STACKS-1);
    numberOfConnectionsOfType[9] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[10] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[11] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);
    numberOfConnectionsOfType[12] = (N_ROWS-1)*(N_COLS-1)*(N_STACKS-1);

    startOfConnectionOfType[0] = 0;
    int numberOfConnections = numberOfConnectionsOfType[0];
    for (int i = 1; i < 13; ++i){
        startOfConnectionOfType[i] = startOfConnectionOfType[i-1] + numberOfConnectionsOfType[i-1];
        numberOfConnections += numberOfConnectionsOfType[i];
    }

    connections = std::vector<Connection>(numberOfConnections);

    // Calculate connections
    int p_index1;
    int p_index2;
    for (int i = 0; i < numberOfConnections; ++i){
        connection2massIndices3D(i, p_index1, p_index2, N_ROWS, N_COLS, N_STACKS);
        connections[i] = Connection(&particles[p_index1], &particles[p_index2]);

        connections[i].setSpringConstant(500.f);
        connections[i].setDamperConstant(5.0f);
        connections[i].setConnectionLength(1.0f);
    }

    //Set length of 2D-diagonal springs to sqrt(2)
    for (int i = startOfConnectionOfType[3]; i < startOfConnectionOfType[9]; ++i){
        connections[i].setConnectionLength(sqrt(2.0f));
    }

    //Set length of 3D-diagonal springs to sqrt(3)
    for (int i = startOfConnectionOfType[9]; i < numberOfConnections; ++i){
        connections[i].setConnectionLength(sqrt(3.0f));
    }
}

void MCS::update(float dt){
    for (int i = 0; i < connections.size(); ++i){
        connections[i].applyForcesToConnectedParticles(dt);
    }

    for (int i = 0; i < particles.size(); ++i){
        particles[i].applyForce(dt);
    }
}

void MCS::rotate(glm::vec3 axisOfRotation, float degrees){

    for (int i = 0; i < particles.size(); ++i){
        Particle& p = particles[i];
        //Cant rotate with glm::rotate???
        
    }
}

void MCS::addRotation(glm::vec3 axisOfRotation, float amount){
    glm::vec3 avgVelBefore = averageVelocity();

    glm::vec3 mid = centerOfMass();
    for (int i = 0; i < particles.size(); ++i){
        Particle& p = particles[i];
        glm::vec3 midToParticle = -p.readPosition() + mid;
        glm::vec3 rotDir = glm::cross(midToParticle, glm::normalize(axisOfRotation));
        p.writeVelocity(p.readVelocity() + rotDir*amount);
    }

    glm::vec3 avgVelAfter = averageVelocity();
    glm::vec3 avgVelError = avgVelAfter - avgVelBefore;
    for (int i = 0; i < particles.size(); ++i){
        //particles[i].writeVelocity(particles[i].readVelocity()-avgVelError);
    }
}

void MCS::setAvgPosition(glm::vec3 pos){
    glm::vec3 avgPos = averagePosition();
    glm::vec3 toAdd = pos - avgPos;
    for (int i = 0; i < particles.size(); ++i){
        particles[i].writePosition(particles[i].readPosition() + toAdd);
    }
}

void MCS::setAvgVelocity(glm::vec3 vel){
    glm::vec3 avgVel = averageVelocity();
    glm::vec3 toAdd = vel - avgVel;
    for (int i = 0; i < particles.size(); ++i){
        particles[i].writeVelocity(particles[i].readVelocity() + toAdd);
    }
}

glm::vec3 MCS::centerOfMass() const{
    glm::vec3 weightedSum(0,0,0);
    float totalMass;
    for (int i = 0; i < particles.size(); ++i){
        weightedSum += particles[i].readPosition()*particles[i].getMass();
        totalMass += particles[i].getMass();
    }
    return weightedSum/totalMass;
}

glm::vec3 MCS::averagePosition() const{
    glm::vec3 sum(0,0,0);
    for (int i = 0; i < particles.size(); ++i){
        sum += particles[i].readPosition();
    }
    return sum/(float)particles.size();
}

glm::vec3 MCS::averageVelocity() const{
    glm::vec3 sum(0,0,0);
    for (int i = 0; i < particles.size(); ++i){
        sum += particles[i].readVelocity();
    }
    return sum/(float)particles.size();
}

//Get-functions
Particle& MCS::getParticle(int index){
    return particles[index];
}

const Connection& MCS::getConnection(int index) const{
    return connections[index];
}

int MCS::getNumberOfParticles(){
    return particles.size();
}

int MCS::getNumberOfConnections(){
    return connections.size();
}

void MCS::connection2massIndices3D(const int connection_index, int &mass_index1, int &mass_index2, const int n_rows, const int n_cols, const int n_stacks){
    // Number of connections of a certain type (13 different directions)
    const int N_TYPES = 13;
    int n_type[N_TYPES];
    int n_connections = 0;

    n_type[0] = n_rows*(n_cols-1)*n_stacks;
    n_type[1] = (n_rows-1)*n_cols*n_stacks;
    n_type[2] = n_rows*n_cols*(n_stacks-1);
    n_type[3] = (n_rows-1)*(n_cols-1)*n_stacks;
    n_type[4] = (n_rows-1)*(n_cols-1)*n_stacks;
    n_type[5] = n_rows*(n_cols-1)*(n_stacks-1);
    n_type[6] = n_rows*(n_cols-1)*(n_stacks-1);
    n_type[7] = (n_rows-1)*n_cols*(n_stacks-1);
    n_type[8] = (n_rows-1)*n_cols*(n_stacks-1);
    n_type[9] = (n_rows-1)*(n_cols-1)*(n_stacks-1);
    n_type[10] = (n_rows-1)*(n_cols-1)*(n_stacks-1);
    n_type[11] = (n_rows-1)*(n_cols-1)*(n_stacks-1);
    n_type[12] = (n_rows-1)*(n_cols-1)*(n_stacks-1);

    // The number of springs of all the previous types with regard to the current type
    int prev_num_springs[N_TYPES];
    for (int i = 0; i < N_TYPES; ++i){
        prev_num_springs[i] = 0;
        // Calculating total number of connections
        n_connections += n_type[i];
    }

    assert(connection_index < n_connections);

    for (int i = 0; i < N_TYPES; ++i){
        for (int j = i; j > 0; --j){
            prev_num_springs[i] += n_type[j-1];
            //std::cout << j << std::endl;
        }
        //std::cout << "n_type" << "[" << i << "] = " << n_type[i] << std::endl;
        //std::cout << "prev_num_springs" << "[" << i << "] = " << prev_num_springs[i] << std::endl;
    }

    // Find out which type the current connection is
    int type;
    for (int i = 0; i < N_TYPES; ++i){
        if(connection_index >= prev_num_springs[i]){
            type = i;
        }
    }

    // Number of mass indices that needs to be added to jump up one row
    int one_row_of_masses = n_cols;

    // Number of mass indices that needs to be added to jump up one stack
    int one_stack_of_masses = n_cols*n_rows;

    // These will be different for each type of connection
    int one_stack_of_connections;
    int one_row_of_connections;

    // These will be set depending on the type and the connection index
    int row_m1;
    int stack_m1;

    switch (type){
        case 0: {
            one_row_of_connections = n_cols-1;
            one_stack_of_connections = n_rows * one_row_of_connections;         

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections);
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections);

            mass_index1 = connection_index - prev_num_springs[type] + row_m1 + stack_m1*n_rows;
            mass_index2 = mass_index1 + 1;
            break;
        }
        case 1: {
            one_row_of_connections = n_cols;
            one_stack_of_connections = (n_rows-1) * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections);
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections);

            mass_index1 = connection_index - prev_num_springs[type] + n_cols*stack_m1;
            mass_index2 = mass_index1 + one_row_of_masses;
            break;
        }
        case 2: {
            one_row_of_connections = n_cols;
            one_stack_of_connections = n_rows * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections);
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections);

            mass_index1 = connection_index - prev_num_springs[type];
            mass_index2 = mass_index1 + one_stack_of_masses;
            break;
        }
        case 3: {
            one_row_of_connections = n_cols-1;
            one_stack_of_connections = (n_rows-1) * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections);
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections);

            mass_index1 = connection_index - prev_num_springs[type] + row_m1 + stack_m1*(n_rows + n_cols - 1);
            mass_index2 = mass_index1 + one_row_of_masses + 1;
            break;
        }
        case 4: {
            one_row_of_connections = n_cols-1;
            one_stack_of_connections = (n_rows-1) * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections);
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections) + 1;

            mass_index1 = connection_index - prev_num_springs[type] + n_cols + row_m1-1 + stack_m1*(n_rows + n_cols - 1);
            mass_index2 = mass_index1 - one_row_of_masses + 1;
            break;
        }
        case 5: {
            one_row_of_connections = n_cols-1;
            one_stack_of_connections = n_rows * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections);
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections);

            mass_index1 = connection_index - prev_num_springs[type] + stack_m1*n_rows + row_m1;
            mass_index2 = mass_index1 + one_stack_of_masses + 1;
            break;
        }
        case 6: {
            one_row_of_connections = n_cols-1;
            one_stack_of_connections = n_rows * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections);
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections);

            mass_index1 = connection_index - prev_num_springs[type] + row_m1 + stack_m1*n_rows + 1;
            mass_index2 = mass_index1 + one_stack_of_masses - 1;
            break;
        }
        case 7: {
            one_row_of_connections = n_cols;
            one_stack_of_connections = (n_rows-1) * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections);
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections);

            mass_index1 = connection_index - prev_num_springs[type] + stack_m1*n_cols;
            mass_index2 = mass_index1 + one_stack_of_masses + one_row_of_connections;
            break;
        }
        case 8: {
            one_row_of_connections = n_cols;
            one_stack_of_connections = (n_rows-1) * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections) + 1;
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections);

            mass_index1 = connection_index - prev_num_springs[type] + (stack_m1-1)*n_cols + one_stack_of_masses;
            mass_index2 = mass_index1 - one_stack_of_masses + one_row_of_masses;
            break;
        }
        case 9: {
            one_row_of_connections = n_cols-1;
            one_stack_of_connections = (n_rows-1) * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections);
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections);

            mass_index1 = connection_index - prev_num_springs[type] + row_m1 + stack_m1*(n_rows + n_cols - 1);
            mass_index2 = mass_index1 + one_stack_of_masses + one_row_of_masses + 1;
            break;

        }
        case 10: {
            one_row_of_connections = n_cols-1;
            one_stack_of_connections = (n_rows-1) * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections) + 1;
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections);

            mass_index1 = connection_index - prev_num_springs[type] + row_m1 + (stack_m1-1)*(n_rows + n_cols - 1) + one_stack_of_masses;
            mass_index2 = mass_index1 - one_stack_of_masses + one_row_of_masses + 1;
            break;
        }
        case 11: {
            one_row_of_connections = n_cols-1;
            one_stack_of_connections = (n_rows-1) * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections);
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections) + 1;

            mass_index1 = connection_index - prev_num_springs[type] + row_m1-1 + stack_m1*(n_rows + n_cols - 1) + one_row_of_masses;
            mass_index2 = mass_index1 + one_stack_of_masses - one_row_of_masses + 1;
            break;
        }
        case 12: {
            one_row_of_connections = n_cols-1;
            one_stack_of_connections = (n_rows-1) * one_row_of_connections;

            stack_m1 = stackOfFirstMass(connection_index, prev_num_springs[type], one_stack_of_connections);
            row_m1 = rowOfFirstMass(connection_index, prev_num_springs[type], one_row_of_connections, one_stack_of_connections);

            mass_index1 = connection_index - prev_num_springs[type] + row_m1 + stack_m1*(n_rows + n_cols - 1) + 1;
            mass_index2 = mass_index1 + one_stack_of_masses + one_row_of_masses - 1;
            break;
        }
        default:
            mass_index1 = mass_index2 = -1;
    }
/*
    std::cout << "n_rows = " << n_rows << std::endl;
    std::cout << "n_cols = " << n_cols << std::endl;
    std::cout << "n_stacks = " << n_stacks << std::endl;
    std::cout << "type = " << type << std::endl;
    std::cout << "one_row_of_connections = " << one_row_of_connections << std::endl;
    std::cout << "one_stack_of_connections = " << one_stack_of_connections << std::endl;
    std::cout << "stack_m1 = " << stack_m1 << std::endl;
    std::cout << "row_m1 = " << row_m1 << std::endl;
*/

}

// Calculate the stack in which the first mass (not necessary mass 1) of the pair is positioned
int MCS::stackOfFirstMass(const int connection_index, const int prev_num_springs, const int one_stack_of_connections){
    // Integer division
    return (connection_index - prev_num_springs)/one_stack_of_connections;
}

// Calculate the row in which first mass (not necessary mass 1) of the pair is positioned
int MCS::rowOfFirstMass(const int connection_index, const int prev_num_springs, const int one_row_of_connections, const int one_stack_of_connections){
    // Integer division
    return ((connection_index - prev_num_springs) % one_stack_of_connections)/one_row_of_connections;
}
