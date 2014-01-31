% Create masses
for row = 1:2
    for col = 1:2
        p = Point;
        p.mass = 1;
        p.pos = [row, col];
        p.vel = [0, 0];
        p.force = [0,0];
        P(row,col) = p;
    end
end

% Create connection
for i=1:6
    C(i) = Connection; 
    C(i).k = 1;
    C(i).b = 1;
    C(i).l = 1;
end
C(3).l = sqrt(2);
C(6).l = sqrt(2);


C(1).p1 = P(1,1);
C(1).p2 = P(2,1);
C(2).p1 = P(1,2);
C(2).p2 = P(2,2);
C(3).p1 = P(1,2);
C(3).p2 = P(2,1);
C(4).p1 = P(1,1);
C(4).p2 = P(1,2);
C(5).p1 = P(2,1);
C(5).p2 = P(2,2);
C(6).p1 = P(1,1);
C(6).p2 = P(2,2);


buffer_index = 1;
C(1).applyForce(buffer_index);
P(1,1)
