function [mi1,mi2] = connection2massIndex(ci,nRows,nCols)

%Number of connections in each direction (|/_\)
num_first = (nRows-1)*nCols; % |
num_second = (nRows-1)*(nCols-1); % /
num_third = nRows*(nCols-1); % _
%num_forth = num_second; % \


if(ci<=num_first) %Direction: |
    %prev_num_springs = 0;
    mi1 = ci;
    mi2 = ci + nCols;
elseif (ci<=(num_first + num_second)) %Direction /
    prev_num_springs = num_first;
    row_m1 = floor((ci - prev_num_springs)/nCols) + 1;
    mi1 = (ci - prev_num_springs) + row_m1;
    mi2 = mi1 + nCols - 1;
elseif (ci<=(num_first + num_second + num_third)) %Direction _
    prev_num_springs = num_first + num_second;
    row_m1 = floor((ci - prev_num_springs)/nCols) + 1;
    mi1 = ci - prev_num_springs + row_m1 - 1;
    mi2 = mi1 + 1;
else %Direction \
    prev_num_springs = num_first + num_second + num_third;
    row_m1 = floor((ci - prev_num_springs)/nCols) + 1;
    mi1 = ci - prev_num_springs + row_m1 - 1;
    mi2 = mi1 + nCols + 1;
end

