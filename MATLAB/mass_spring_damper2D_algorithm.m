% Allokering av minne
clear all;
nRows = 4;
nCols = 4;
SIZE = [nRows nCols];

g = 9.82;

%number of masses:
number_of_masses = prod(SIZE);
%number of connections:        -                |                 / and \
number_of_connections = (nRows-1)*nCols + nRows*(nCols-1) + 2*(nRows-1)*(nCols-1);

%--Masses list--
masses = ones(number_of_masses,1);
positions = zeros([number_of_masses, 2, 2]);
velocities = zeros([number_of_masses, 2, 2]);

for j=1:number_of_masses % For each mass
    positions(j,:,1) = [floor((j-1)/nCols), rem(j-1,nCols)];
end

%positions(1,:,1) = [0, -0.5];


% Create list with indices to neighbors and connections
max_number_of_neighbors = 8; % 8 i två dimensioner
connection_index = zeros(number_of_masses, max_number_of_neighbors);
neighbor_index = zeros(prod(SIZE), max_number_of_neighbors);


%Calculate connection indices and mass indices of neighbors
for j=1:number_of_masses % For each mass
    for z=1:max_number_of_neighbors % For each neighbor
        connection_index(j,z) = connectionInDir(j, z, nRows, nCols);
        neighbor_index(j,z) = massIndexOfNeighbor(j,z, nRows, nCols);
    end
end

%--Connections list--
spring_constants = 2000 * ones(number_of_connections, 1);
damper_constants = 5 * ones(number_of_connections, 1);
spring_length = ones(number_of_connections, 1);

spring_length(nRows*(nCols-1)+1:nRows*(nCols-1) + (nRows-1)*(nCols-1)) = sqrt(2);
spring_length(nRows*(nCols-1) + (nRows-1)*(nCols-1) + (nRows-1)*nCols + 1:number_of_connections) = sqrt(2);


close all;

%% Startvärden. Simulering
read_buffer_index = 1;
write_buffer_index = 2;

velocities(:,:,:) = 0;
for j=1:number_of_masses % For each mass
    positions(j,:,read_buffer_index) = [15 + floor((j-1)/nCols), rem(j-1,nCols)];
end
%positions(1,:,read_buffer_index) = [0, -1];
velocities(1,:,read_buffer_index) = [5, -5];
velocities(number_of_masses,:,read_buffer_index) = [-5, 5];

n_frames = 5000;
T = 0.01;

figure;
%pause(2);
%hold on;
for i=1:n_frames %Loop through frames
	tic;
    %tic;
    for j=1:number_of_masses %Loop through masses        
        F = [0,0];
        for dir=1:max_number_of_neighbors %Loop through neighbors in all directions
            con_ind = connection_index(j,dir);
            neigh_ind = neighbor_index(j,dir);
            
            if neigh_ind ~= -1 && con_ind ~= -1
                %Spring properties
                k = spring_constants(con_ind);
                b = damper_constants(con_ind);
                l = spring_length(con_ind);

                %Vector from neighbors position to this mass position
                pos = positions(j, :, read_buffer_index);
                neigh_pos = positions(neigh_ind, :, read_buffer_index);
                deltaP = pos - neigh_pos;
                
                %Only for plot
                %xVec = [pos(1), neigh_pos(1)];
                %yVec = [pos(2), neigh_pos(2)];
                %plot(yVec, xVec);
                
                
                norm_deltaP = norm(deltaP);
                if(norm_deltaP == 0)
                    deltaP_hat = [0,0];
                else
                    deltaP_hat = deltaP/norm_deltaP;
                end

                %Vector from neighbors velocity to this mass velocity
                vel = velocities(j, :, read_buffer_index);
                neigh_vel = velocities(neigh_ind, :, read_buffer_index);
                deltaV = vel - neigh_vel;

                %Calculate force
                F = F + (-k*(norm(deltaP) - l) - b*dot(deltaV, deltaP_hat))*deltaP_hat;
            end
        end
        
        %Add the Gravity force
        F = F + masses(j)*g*[-1,0];
        
        %Calculacte acceleration, velocity and position
        a = F/masses(j);
        v = velocities(j, :,read_buffer_index) + T*a;
        p = positions(j, :,read_buffer_index) + T*v;
        
        %Check collision with y=0
        if p(1) < 0
            p(1) = 0;
            v(1) = -v(1); 
        end
        
        %Store information in backbuffer
        velocities(j, :,write_buffer_index) = v;
        positions(j, :,write_buffer_index) = p;
    end
    plot(positions(:,2,write_buffer_index), ...
            positions(:,1,write_buffer_index),'*');
    axis manual;
    axis([-2 5 0 20]);
    pause(max(T-toc,0.001));
    
    %Swap buffer
    read_buffer_index = rem(read_buffer_index,2)+1;
    write_buffer_index = rem(write_buffer_index,2)+1;
    %clf('reset');
    %toc;
end
