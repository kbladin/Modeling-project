%Returns connection index to the connection 
%in the direction specified

function linearConnectionIndex = connectionInDir(mass_index, dir, nRows, nCols)

neighbor_index = massIndexOfNeighbor(mass_index, dir, nRows, nCols);

if(neighbor_index > 0)
        [connection_list_index, connection_index] = ...
        massIndices2connectionIndices(mass_index, neighbor_index, nCols);
    if (connection_list_index > 0 && connection_index > 0)

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
    else
        %No neighbor in this direction
        linearConnectionIndex = -1;
    end
else
    %No neighbor in this direction
    linearConnectionIndex = -1;
end

end