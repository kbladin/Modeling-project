% input: i = index to one mass
% input: j = index to neighbor mass
% input: nCols = number of columns
% output: d = direction, used for connection list index
% output: c = connection index
function [d, c] = massIndices2connectionIndices(i,j,nCols)
d = -1;
c = -1;

if(nCols <= 2)
    error('This function only supports nCols > 2');
end


%{

%This will work if nCols > 2
if(abs(i-j) == 1)
    % connection orientation: -
    if rem(min(i,j),nCols) ~= 0
        d = 3;
        c = min(i,j) - floor(min(i,j)/nCols);
    else
        return
    end
elseif(abs(i-j) == nCols-1)
    % connection orientation: /
    if rem(min(i,j)-1,nCols) ~= 0
        d = 2;
        c = min(i,j) - floor(max(i,j)/nCols);
    end
elseif(abs(i-j) == nCols)
    % connection orientation: |
        d = 1;
        c = min(i,j);
elseif(abs(i-j) == nCols+1 )
    % connection orientation: \
    if rem(min(i,j),nCols) ~= 0
        d = 4;
        c = min(i,j) - ceil(min(i,j)/nCols) + 1;
    end
else
    error(['|i-j| = ', num2str(abs(i-j)) ' -> No such neighbor']);
end


%}


%{
switch abs(i-j)
    case 1% connection orientation: -
        if rem(min(i,j),nCols) ~= 0
            d = 3;
            c = min(i,j) - floor(min(i,j)/nCols);
        else
            return
        end
    case nCols-1
        % connection orientation: /
        if rem(min(i,j)-1,nCols) ~= 0
            d = 2;
            c = min(i,j) - floor(max(i,j)/nCols);
        end
    case nCols
        % connection orientation: |
            d = 1;
            c = min(i,j);
    case nCols+1
        % connection orientation: \
        if rem(min(i,j),nCols) ~= 0
            d = 4;
            c = min(i,j) - ceil(min(i,j)/nCols) + 1;
    end
    otherwise
        error(['|i-j| = ', num2str(abs(i-j)) ' -> No such neighbor']);
end

end
%}

switch abs(i-j)
    case 1
        % connection orientation: -
        if rem(min(i,j),nCols) ~= 0
            d = 3;
            c = min(i,j) - floor(min(i,j)/nCols);
        else
            return
        end
    case nCols-1
        % connection orientation: /
        if rem(min(i,j)-1,nCols) ~= 0
            d = 2;
            c = min(i,j) - floor(max(i,j)/nCols);
        end
    case nCols
        % connection orientation: |
        d = 1;
        c = min(i,j);
    case nCols+1
        % connection orientation: \
        if rem(min(i,j),nCols) ~= 0
            d = 4;
            c = min(i,j) - ceil(min(i,j)/nCols) + 1;
        end
    otherwise
        error(['|i-j| = ', num2str(abs(i-j)) ' -> No such neighbor']);
end

end

