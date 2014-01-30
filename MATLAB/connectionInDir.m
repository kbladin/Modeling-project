%Returns connection index to the connection 
%in the direction specified

function linearConnectionIndex = connectionInDir(mass_index, dir, nRows, nCols)

%{
% Calculate cartesian index
neighbor_pos = [row, col] + neighborIndex2offset(dir);

% Store y and x index as nice variables
neighbor_row = neighbor_pos(1);
neighbor_col = neighbor_pos(2);

neighbor_index = sub2ind([nCols,nRows],neighbor_col,neighbor_row)

%Check if neighbor exist
if 0<neighbor_row && neighbor_row<=nRows && ...
   0<neighbor_col && neighbor_col<=nCols

    [connection_list_index, connection_index] = ...
        massIndices2ConnectionIndices(mass_index, neighbor_index, nCols);
else
    error('No neighbor in this direction!'); 
end

%}


neighbor_index = massIndexOfNeighbor(mass_index, dir, nRows, nCols);

[connection_list_index, connection_index] = ...
    massIndices2ConnectionIndices(mass_index, neighbor_index, nCols);


%Number of horizontal connections
nHor = nRows*(nCols - 1);
%Number of diagonal connections
nDiag = 2*(nCols - 1)*(nRows - 1);
%Number of vertical connections
nVert = nCols*(nRows - 1);

switch connection_list_index
    case 1
        linearConnectionIndex = connection_index;
    case 2
        linearConnectionIndex = connection_index + nVert;
    case 3
        linearConnectionIndex = connection_index + nVert + nDiag/2;
    case 4
        linearConnectionIndex = connection_index + nVert + nDiag/2 + nHor;
    otherwise
        error('Invalid connection list index!'); 
end

