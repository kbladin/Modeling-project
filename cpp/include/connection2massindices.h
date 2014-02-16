#ifndef CONNECTION_2_MASSINDICES_
#define CONNECTION_2_MASSINDICES_

#include <assert.h> 
#include <iostream>
#include "glm/glm.hpp"

void connection2massIndices(const int connection_index, int &mass_index1, int &mass_index2, const int n_rows, const int n_cols);

void connection2massIndices3D(const int connection_index, int &mass_index1, int &mass_index2, const int n_rows, const int n_cols, const int n_stacks);

int stackOfFirstMass(const int connection_index, const int prev_num_springs, const int one_stack_of_connections);

int rowOfFirstMass(const int connection_index, const int prev_num_springs, const int one_row_of_connections, const int one_stack_of_connections);

#endif