% input: i = index to one mass
% input: j = index to neighbor mass
% input: nCols = number of columns
% output: d = direction, used for connection list index
% output: c = connection index
function [d, c] = massIndices2connectionIndices(i,j,nCols)
d = -1;
c = -1;
    switch abs(i-j)
        
        % connection orientation: -
        case 1 
            if rem(min(i,j),nCols) ~= 0
                d = 3;
            else
                return
            end
        
        % connection orientation: /
        case nCols-1 
            if rem(min(i,j)-1,nCols) ~= 0
                d = 2;
                c = min(i,j)-1;
            end
            return
            
        % connection orientation: |
        case nCols 
            d = 1;
            
        % connection orientation: \
        case nCols+1 
            if rem(min(i,j),nCols) ~= 0
                d = 4;
            else
                return
            end
            
        otherwise
            error(['|i-j| = ', num2str(abs(i-j)) ' -> No such neighbor']);
    end
c = min(i,j);
end
