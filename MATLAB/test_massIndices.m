nRows = 4;
nCols = 9;
SIZE = [nRows nCols];
number_of_masses = prod(SIZE);

max_number_of_neighbors = 8;
neighbor_index = zeros(prod(SIZE), max_number_of_neighbors);

connection_count = 0;

for i=1:number_of_masses % For each mass
    
    %Compute cartesian index
    [col, row] = ind2sub([nCols,nRows],i);
    
    %Calculate neighbor indices
    for neighbor_j=1:max_number_of_neighbors % For each neighbor
        % Calculate cartesian index
        neighbor_pos = [row, col] + neighborIndex2offset(neighbor_j);
        
        % Store y and x index as nice variables
        neighbor_row = neighbor_pos(1);
        neighbor_col = neighbor_pos(2);
        
        %Check if neighbor exist
        if 0<neighbor_row && neighbor_row<=nRows && ...
           0<neighbor_col && neighbor_col<=nCols
            % Add linear index in neighbor index list
            neighbor_index(i,neighbor_j) = sub2ind([nCols,nRows],neighbor_col,neighbor_row);
            
            [direction,c] = massIndices2connectionIndices(i,neighbor_index(i,neighbor_j),nCols)
        end
    end
end
