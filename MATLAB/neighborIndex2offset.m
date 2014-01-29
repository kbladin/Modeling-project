% Insert an index and get the offset to the neighbor
% in both directions.
% In 2D, the indices have the following configuration: 
% 8 1 2
%  \|/
% 7-o-3
%  /|\
% 6 5 4
%
% x ->
% y
% |
% v

function res = neighborIndex2offset(i)

switch i
    case 1
        x = 0;
        y = -1;
    case 2
        x = 1;
        y = -1;
    case 3
        x = 1;
        y = 0;
    case 4
        x = 1;
        y = 1;
    case 5
        x = 0;
        y = 1;
    case 6
        x = -1;
        y = 1;
    case 7
        x = -1;
        y = 0;
    case 8
        x = -1;
        y = -1;
    otherwise
        error('Invalid input (1<=i<=8)');
end

res = [y,x];