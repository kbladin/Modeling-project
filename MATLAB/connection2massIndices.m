function [mi1,mi2] = connection2massIndices(ci,nRows,nCols)

%Number of connections in each direction (|/_\)
n_type1 = (nRows-1)*nCols; % |
n_type2 = (nRows-1)*(nCols-1); % /
n_type3 = nRows*(nCols-1); % _
%n_type4 = n_type2; % \


if(ci<=n_type1) %Direction: |
    %prev_num_springs = 0;
    mi1 = ci;
    mi2 = ci + nCols;
elseif (ci<=(n_type1 + n_type2)) %Direction /
    prev_num_springs = n_type1;
    row_m1 = ceil((ci - prev_num_springs)/(nCols-1));
    mi1 = (ci - prev_num_springs) + row_m1;
    mi2 = mi1 + nCols - 1;
elseif (ci<=(n_type1 + n_type2 + n_type3)) %Direction _
    prev_num_springs = n_type1 + n_type2;
    row_m1 = ceil((ci - prev_num_springs)/(nCols-1));
    mi1 = ci - prev_num_springs + row_m1 - 1;
    mi2 = mi1 + 1;
else %Direction \
    prev_num_springs = n_type1 + n_type2 + n_type3;
    row_m1 = ceil((ci - prev_num_springs)/(nCols-1));
    mi1 = ci - prev_num_springs + row_m1 - 1;
    mi2 = mi1 + nCols + 1;
end

