for row = 1:3
    for col = 1:4
        m = Point;
        
        m.mass = 1;
        m.pos = [row, col;  %front buffer
                 0  , 0 ];  %back buffer
             
        m.vel = [0, 0;      %front buffer
                 0, 0];     %back buffer

        M(row,col) = m;
    end
end