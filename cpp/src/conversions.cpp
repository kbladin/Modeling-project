#include "conversions.h"

void connection2massIndices(const int connection_index, int &mass_index1, int &mass_index2, const int n_rows, const int n_cols, const int n_stacks){
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

void vertex2particleIndex(int vertexIndex, int &particleIndex, const int n_rows, const int n_cols, const int n_stacks){
    int Ntype0 = n_rows * n_cols;         //back
    int Ntype1 = Ntype0;                          //front
    int Ntype2 = n_rows * n_stacks;       //left
    int Ntype3 = Ntype2;                          //right
    int Ntype4 = n_stacks * n_cols;       //bottom
    int Ntype5 = Ntype4;                          //top
    int TotNtype = Ntype0 + Ntype1 + Ntype2 + Ntype3 + Ntype4 + Ntype5;
    
    assert(vertexIndex < TotNtype);
    
    int oneRowOfParticles = n_cols;
    int oneStackOfParticles = n_rows*n_cols;
    int newVertexIndex;
    
    if(vertexIndex < Ntype0){                                         //back
        particleIndex = vertexIndex;
    }
    else if(vertexIndex < Ntype0 + Ntype1){                           //front
        newVertexIndex = vertexIndex - Ntype0;
        particleIndex = newVertexIndex + n_cols*n_rows*(n_stacks-1);
    }
    else if(vertexIndex < Ntype0 + Ntype1 + Ntype2){                  //left
        newVertexIndex = vertexIndex - Ntype0 - Ntype1;
        particleIndex = (newVertexIndex % n_stacks)*oneStackOfParticles + (newVertexIndex/n_stacks)*oneRowOfParticles;
    }
    else if(vertexIndex < Ntype0 + Ntype1 + Ntype2 + Ntype3){         //right
        newVertexIndex = vertexIndex - Ntype0 - Ntype1 - Ntype2;
        particleIndex = (newVertexIndex % n_stacks)*oneStackOfParticles + (newVertexIndex/n_stacks)*oneRowOfParticles + oneRowOfParticles-1;
    }
    else if(vertexIndex < Ntype0 + Ntype1 + Ntype2 + Ntype3 + Ntype4){//bottom
        newVertexIndex = vertexIndex - Ntype0 - Ntype1 - Ntype2 - Ntype3;
        particleIndex = (newVertexIndex / n_cols)*oneStackOfParticles + newVertexIndex % n_cols;
    }
    else if(vertexIndex < TotNtype){                                  //top
        newVertexIndex = vertexIndex - Ntype0 - Ntype1 - Ntype2 - Ntype3 - Ntype4;
        particleIndex = (newVertexIndex / n_cols)*oneStackOfParticles + newVertexIndex % n_cols + (n_rows-1)*n_cols;
    }
}

void triangle2vertexIndices(int triangleIndex, int &vertexIndex1, int &vertexIndex2, int &vertexIndex3, const int n_rows, const int n_cols, const int n_stacks){
    int Ntype0 = 2*(n_rows-1)*(n_cols-1);         //back
    int Ntype1 = Ntype0;                          //front
    int Ntype2 = 2*(n_rows-1)*(n_stacks-1);       //left
    int Ntype3 = Ntype2;                          //right
    int Ntype4 = 2*(n_stacks-1)*(n_cols-1);       //bottom
    int Ntype5 = Ntype4;                          //top
    int TotNtype = Ntype0 + Ntype1 + Ntype2 + Ntype3 + Ntype4 + Ntype5;
    
    int Ntype_verts0 = n_rows * n_cols;         //back
    int Ntype_verts1 = Ntype_verts0;                          //front
    int Ntype_verts2 = n_rows * n_stacks;       //left
    int Ntype_verts3 = Ntype_verts2;                          //right
    int Ntype_verts4 = n_stacks * n_cols;       //bottom
    
    
    assert(triangleIndex < TotNtype);
    
    int oneRowOfParticlesLocal;
    
    int newTriangleIndex;
    
    int row_p1 = (n_rows == 1) ? 0 : triangleIndex/(Ntype0/(n_rows-1));
    
    if(triangleIndex < Ntype0){                                         //back
        oneRowOfParticlesLocal = n_cols;
        if(triangleIndex%2==0){     //even
            vertexIndex1=triangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal;
            vertexIndex3=vertexIndex2+1;
        }
        else{                       //odd
            vertexIndex1=(triangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex1+1;
        }
    }
    else if(triangleIndex < Ntype0 + Ntype1){                           //front
        oneRowOfParticlesLocal = n_cols;
        newTriangleIndex = triangleIndex - Ntype0;
        int row_p1 = floor(newTriangleIndex/floor(Ntype1/(n_rows-1)));
        
        if(newTriangleIndex%2==0){  //even
            vertexIndex1=newTriangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex2-1;
        }
        else{                       //odd
            vertexIndex1=(newTriangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+1;
            vertexIndex3=vertexIndex2+oneRowOfParticlesLocal;
        }
        vertexIndex1 += Ntype_verts0;
        vertexIndex2 += Ntype_verts0;
        vertexIndex3 += Ntype_verts0;
    }
    else if(triangleIndex < Ntype0+Ntype1+Ntype2){                  //left
        oneRowOfParticlesLocal = n_stacks;
        newTriangleIndex=triangleIndex-(Ntype0+Ntype1);
        int row_p1 = floor(newTriangleIndex/floor(Ntype2/(n_rows-1)));
        
        if(newTriangleIndex%2==0){  //even
            vertexIndex1=newTriangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex2-1;
        }
        else{                       //odd
            vertexIndex1=(newTriangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+1;
            vertexIndex3=vertexIndex2+oneRowOfParticlesLocal;
        }
        vertexIndex1 += Ntype_verts0 + Ntype_verts1;
        vertexIndex2 += Ntype_verts0 + Ntype_verts1;
        vertexIndex3 += Ntype_verts0 + Ntype_verts1;
    }
    else if(triangleIndex < Ntype0+Ntype1+Ntype2+Ntype3){           //right
        oneRowOfParticlesLocal = n_stacks;
        newTriangleIndex = triangleIndex - (Ntype0+Ntype1+Ntype2);
        int row_p1 = floor(newTriangleIndex/floor(Ntype3/(n_rows-1)));
        
        if(newTriangleIndex%2==0){     //even
            vertexIndex1=newTriangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal;
            vertexIndex3=vertexIndex2+1;
        }
        else{                       //odd
            vertexIndex1=(newTriangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex1+1;
        }
        vertexIndex1 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2;
        vertexIndex2 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2;
        vertexIndex3 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2;
    }
    else if(triangleIndex < Ntype0+Ntype1+Ntype2+Ntype3+Ntype4){    //bottom
        oneRowOfParticlesLocal = n_cols;
        newTriangleIndex = triangleIndex - (Ntype0+Ntype1+Ntype2+Ntype3);
        int row_p1 = floor(newTriangleIndex/floor(Ntype4/(n_stacks-1)));
        if(newTriangleIndex%2==0){  //even
            vertexIndex1=newTriangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex2-1;
        }
        else{                       //odd
            vertexIndex1=(newTriangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+1;
            vertexIndex3=vertexIndex2+oneRowOfParticlesLocal;
        }
        vertexIndex1 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3;
        vertexIndex2 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3;
        vertexIndex3 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3;
    }
    else if(triangleIndex < TotNtype){                                  //top
        oneRowOfParticlesLocal = n_cols;
        newTriangleIndex = triangleIndex - (Ntype0+Ntype1+Ntype2+Ntype3+Ntype4);
        int row_p1 = floor(newTriangleIndex/floor(Ntype5/(n_stacks-1)));
        if(newTriangleIndex%2==0){     //even
            vertexIndex1=newTriangleIndex/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal;
            vertexIndex3=vertexIndex2+1;
        }
        else{                       //odd
            vertexIndex1=(newTriangleIndex-1)/2+row_p1;
            vertexIndex2=vertexIndex1+oneRowOfParticlesLocal+1;
            vertexIndex3=vertexIndex1+1;
        }
        vertexIndex1 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3 + Ntype_verts4;
        vertexIndex2 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3 + Ntype_verts4;
        vertexIndex3 += Ntype_verts0 + Ntype_verts1 + Ntype_verts2 + Ntype_verts3 + Ntype_verts4;
    }
}
