% Allokering av minne
nRows = 2;
nCols = 3;
SIZE = [nRows nCols];
number_of_masses = prod(SIZE);

masses = ones(number_of_masses,1);

%number of connections:        -                |                 / and \
number_of_connections = (nRows-1)*nCols + nRows*(nCols-1) + 2*(nRows-1)*(nCols-1);

spring_constants = 200*ones(SIZE-1);
spring_length = 4*ones(SIZE-1);
damper_constants = 6*ones(SIZE-1);


positions = zeros([SIZE 2]);
velocities = zeros([SIZE 2]);


% skapa lista med index till grannar
max_number_of_neighbors = 8;
connection_index = zeros(prod(SIZE), max_number_of_neighbors);
neighbor_index = zeros(prod(SIZE), max_number_of_neighbors);



%% Calculate neighbor indices
for j=1:number_of_masses % For each mass
    
    %Compute cartesian index
    [col, row] = ind2sub([nCols,nRows],j);
    
    for z=1:max_number_of_neighbors % For each neighbor
        % Calculate cartesian index
        neighbor_pos = [row, col] + neighborIndex2offset(z);
        
        % Store y and x index as nice variables
        neighbor_row = neighbor_pos(1);
        neighbor_col = neighbor_pos(2);
        
        %Check if neighbor exist
        if 0<neighbor_row && neighbor_row<=nRows && ...
           0<neighbor_col && neighbor_col<=nCols
            % Add linear index in neighbor index list
            neighbor_index(j,z) = sub2ind([nCols,nRows],neighbor_col,neighbor_row);
        end
    end
end

%% Calculate connection indices
% TODO fixa linjära indexlistor till till connections




%%
close all;

% Startvärden
read_buffer_index = 1;
write_buffer_index = 2;

positions(:,1) = 4*(1:SIZE);
velocities(:,1) = zeros(1,SIZE);

positions(2:SIZE,1) = positions(2:SIZE)-1;

%positions(1,1) = -1;


n_frames = 500;
T = 0.05;
t = T*(0:n_frames-1);

for i=1:n_frames %Loop through frames
    t=t+T;
    for j=1:SIZE %Loop through masses
        
        positions(1,1)=1;
        
        F = 0;
        
        for k=1:max_number_of_neighbors %Loop through neighbors
            %Calculate force
            j_neighbor = neighbor_index(j,k);
            if j_neighbor ~= 0
                connection_index = min([j, j_neighbor]);
                
                delta_p = positions(j_neighbor,read_buffer_index)-positions(j,read_buffer_index);
                delta_v = velocities(j_neighbor,read_buffer_index)-velocities(j,read_buffer_index);
                
                direction = sign(delta_p);
                
                Fk = -direction*spring_constants(connection_index)*(spring_length(connection_index)-abs(delta_p));
                Fb = damper_constants(connection_index)*delta_v;
                
                F = F + (Fk+Fb);
            end
        end
        
        %Calculacte acceleration, velocity and position
        a = F/masses(j);
        v = velocities(j,read_buffer_index) + T*a;
        p = positions(j,read_buffer_index) + T*v;
        
        %Store information in backbuffer
        velocities(j,write_buffer_index) = v;
        positions(j,write_buffer_index) = p;
    
    end
    
    
    plot(ones(1,SIZE),positions(:,write_buffer_index),'*');
    hold off;
    axis manual;
    axis([0 2 -1 31]);
    pause(T);
    
    read_buffer_index = rem(read_buffer_index,2)+1;
    write_buffer_index = rem(write_buffer_index,2)+1; 
end




