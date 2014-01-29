% Allokering av minne
N = 5;
masses = ones(N,1);
spring_constants = 200*ones(N-1,1);
spring_length = 4*ones(N-1,1);
damper_constants = 6*ones(N-1,1);


positions = zeros(N,2);
velocities = zeros(N,2);


% skapa lista med index till grannar
max_number_of_neighbors = 2;
neighbor_index = zeros(N,max_number_of_neighbors);
neighbor_index(1,:) = [0,2];
neighbor_index(N,:) = [N-1,0];
for j=2:N-1
    neighbor_index(j,:) = [j-1,j+1];
end




%%
close all;

% Startvärden
read_buffer_index = 1;
write_buffer_index = 2;

positions(:,1) = 4*(1:N);
velocities(:,1) = zeros(1,N);

positions(2:N,1) = positions(2:N)-1;

%positions(1,1) = -1;


n_frames = 500;
T = 0.05;
t = T*(0:n_frames-1);

for i=1:n_frames %Loop through frames
    t=t+T;
    for j=1:N %Loop through masses
        
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
    
    
    plot(ones(1,N),positions(:,write_buffer_index),'*');
    hold off;
    axis manual;
    axis([0 2 -1 31]);
    pause(T);
    
    read_buffer_index = rem(read_buffer_index,2)+1;
    write_buffer_index = rem(write_buffer_index,2)+1; 
end




