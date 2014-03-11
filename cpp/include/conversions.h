#ifndef CONVERSIONS_H_
#define CONVERSIONS_H_

#include <assert.h> 
#include <iostream>
#include "glm/glm.hpp"

void connection2massIndices(const int connection_index,
                            int &mass_index1,
                            int &mass_index2,
                            const int n_rows,
                            const int n_cols,
                            const int n_stacks);

int stackOfFirstMass(const int connection_index,
                     const int prev_num_springs,
                     const int one_stack_of_connections);

int rowOfFirstMass(const int connection_index,
                   const int prev_num_springs,
                   const int one_row_of_connections,
                   const int one_stack_of_connections);

void vertex2particleIndex(int vertexIndex,
                          int &particleIndex,
                          const int n_rows,
                          const int n_cols,
                          const int n_stacks);

void triangle2vertexIndices(int triangleIndex,
                            int &vertexIndex1,
                            int &vertexIndex2,
                            int &vertexIndex3,
                            const int n_rows,
                            const int n_cols,
                            const int n_stacks);

#endif