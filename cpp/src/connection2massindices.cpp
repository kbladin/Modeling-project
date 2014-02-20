#include "connection2massindices.h"

void connection2massIndices(const int connection_index, int &mass_index1, int &mass_index2, const int n_rows, const int n_cols)
{
	//Number of connections in each direction (|/_\)
	int n_type1 = (n_rows-1)*n_cols; // |
	int n_type2 = (n_rows-1)*(n_cols-1); // /
	int n_type3 = n_rows*(n_cols-1); // _
	int n_type4 = n_type2; // \

	int prev_num_springs;
	int row_m1;

	if (connection_index < n_type1) //Direction: |
	{
	    //prev_num_springs = 0;
	    mass_index1 = connection_index;
	    mass_index2 = connection_index + n_cols;
	}
	else if (connection_index < (n_type1 + n_type2)) //Direction /
	{
	    prev_num_springs = n_type1;
	    row_m1 = glm::floor((connection_index - prev_num_springs)/(n_cols-1));
	    mass_index1 = connection_index - prev_num_springs + row_m1;
	    mass_index2 = mass_index1 + n_cols + 1;
	}
	else if (connection_index < (n_type1 + n_type2 + n_type3)) //Direction _
	{
	    prev_num_springs = n_type1 + n_type2;
	    row_m1 = glm::floor((connection_index - prev_num_springs)/(n_cols-1));
	    mass_index1 = connection_index - prev_num_springs + row_m1;
	    mass_index2 = mass_index1 + 1;
	}
	else // Direction \ .
	{
		prev_num_springs = n_type1 + n_type2 + n_type3;
	    row_m1 = glm::floor((connection_index - prev_num_springs)/(n_cols-1));
	    mass_index1 = connection_index - prev_num_springs + row_m1 + 1;
	    mass_index2 = mass_index1 + n_cols - 1;
	}
}

void connection2massIndices3D(const int connection_index, int &mass_index1, int &mass_index2, const int n_rows, const int n_cols, const int n_stacks){
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
int stackOfFirstMass(const int connection_index, const int prev_num_springs, const int one_stack_of_connections){
	// Integer division
	return (connection_index - prev_num_springs)/one_stack_of_connections;
}

// Calculate the row in which first mass (not necessary mass 1) of the pair is positioned
int rowOfFirstMass(const int connection_index, const int prev_num_springs, const int one_row_of_connections, const int one_stack_of_connections){
	// Integer division
	return ((connection_index - prev_num_springs) % one_stack_of_connections)/one_row_of_connections;
}
