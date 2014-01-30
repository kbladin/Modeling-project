
function massIndex_neighbor = massIndexOfNeighbor(massIndex, dir, nRows, nCols)

%Compute cartesian index
[col, row] = ind2sub([nCols,nRows],massIndex);

% Calculate cartesian index
neighbor_pos = [row, col] + neighborIndex2offset(dir);

% Store y and x index as nice variables
neighbor_row = neighbor_pos(1);
neighbor_col = neighbor_pos(2);

%Check if neighbor exist
if 0<neighbor_row && neighbor_row<=nRows && ...
   0<neighbor_col && neighbor_col<=nCols

    massIndex_neighbor = sub2ind([nCols,nRows],neighbor_col,neighbor_row);
else
    error('No neighbor in this direction!'); 
end