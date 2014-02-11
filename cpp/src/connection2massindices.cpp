#include <iostream>

#include "connection2massindices.h"
#include "glm/glm.hpp"

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