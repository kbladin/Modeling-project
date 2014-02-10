% Allokering av minne
clear all;
nRows = 2;
nCols = 2;

g = 9.82;

number_of_masses = nRows*nCols;

n_type1 = (nRows-1)*nCols; % |
n_type2 = (nRows-1)*(nCols-1); % /
n_type3 = nRows*(nCols-1); % _
n_type4 = n_type2; % \

number_of_connections = n_type1 + n_type2 + n_type3 + n_type4;

%--Masses list--
masses = ones(number_of_masses,1);
positions = zeros([number_of_masses, 2, 2]);
velocities = zeros([number_of_masses, 2, 2]);
forces = zeros([number_of_masses, 2]);

for j=1:number_of_masses % For each mass
    positions(j,:,1) = [floor((j-1)/nCols), rem(j-1,nCols)];
end

%Calculate connection2massIndices
connected_masses = zeros([number_of_connections, 2]);
for i=1:number_of_connections
    [mass_index1, mass_index2] = connection2massIndices(i,nRows,nCols);
    connected_masses(i,1) = mass_index1;
    connected_masses(i,2) = mass_index2;
end

%--Connections list--
spring_constants = 2000 * ones(number_of_connections, 1);
damper_constants = 5 * ones(number_of_connections, 1);
spring_lengths = ones(number_of_connections, 1);

type_2_range = n_type1+1 : n_type1+n_type2;
type_4_range = n_type1+n_type2+n_type3+1 : number_of_connections;
spring_lengths(type_2_range) = sqrt(2);
spring_lengths(type_4_range) = sqrt(2);


%% Startvärden. Simulering
close all;

read_buffer_index = 1;
write_buffer_index = 2;

velocities(:,:,:) = 0;
for j=1:number_of_masses % For each mass
    positions(j,:,read_buffer_index) = [15 + floor((j-1)/nCols), rem(j-1,nCols)];
end

%Add rotation to the body
velocities(1,:,read_buffer_index) = [5, -5];
velocities(number_of_masses,:,read_buffer_index) = [-5, 5];

n_frames = 1000;
T = 0.01;

figure;
%pause(2);

for i=1:n_frames %Loop through frames
	tic;
    hold on;
    
    %{
    forces_from_connectios = calculateForce(...
        spring_constants,...
        damper_constants,...
        spring_lengths,...
        positions(connected_masses(:,1),:,read_buffer_index),...
        positions(connected_masses(:,2),:,read_buffer_index),...
        velocities(connected_masses(:,1),:,read_buffer_index),...
        velocities(connected_masses(:,2),:,read_buffer_index));
    
    for connection_index=1:number_of_connections %Loop through connections and apply forces
        forces(connected_masses(connection_index,1),:) = forces(connected_masses(connection_index,1),:) + forces_from_connectios(connection_index,:);
        forces(connected_masses(connection_index,2),:) = forces(connected_masses(connection_index,2),:) - forces_from_connectios(connection_index,:);
    end
    %}
    %%{
    for connection_index=1:number_of_connections %Loop through connections
        % Spring and damper properties
        k = spring_constants(connection_index);
        b = damper_constants(connection_index);
        l = spring_lengths(connection_index);
        
        % position
        mass_index1 = connected_masses(connection_index,1);
        mass_index2 = connected_masses(connection_index,2);
        p1 = positions(mass_index1,:,read_buffer_index);
        p2 = positions(mass_index2,:,read_buffer_index);
        delta_p = p1 - p2;
        
        %Normalize delta_p
        norm_delta_p = norm(delta_p);
        if(norm_delta_p == 0)
            delta_p_hat = [0,0];
        else
            delta_p_hat = delta_p/norm_delta_p;
        end
        
        %Only for plot
        %%{
        xVec = [p1(1), p2(1)];
        yVec = [p1(2), p2(2)];
        r = 5*abs(norm_delta_p-l);
        r = min(max(r,0),1);
        color = [r 1-r 0];
        plot(yVec, xVec, 'Color', color);
        
        p = 0.5*[p1 + p2];
        text(0.2+p(2),p(1),num2str(connection_index));
        
        %%}
        
        % velocities
        v1 = velocities(mass_index1,:,read_buffer_index);
        v2 = velocities(mass_index2,:,read_buffer_index);
        delta_v = v1 - v2;
        
        %Calculate force from connections
        force_from_connection = (-k*(norm(delta_p) - l) - b*dot(delta_v, delta_p_hat))*delta_p_hat;
        forces(mass_index1,:) = forces(mass_index1,:) + force_from_connection;
        forces(mass_index2,:) = forces(mass_index2,:) - force_from_connection;
                
    end
    %%}
    
    
    %Calculacte acceleration, velocity and position
    for mass_index=1:number_of_masses
        
        %   connection forces                       gravity
        a = forces(mass_index,:)/masses(mass_index) + g*[-1,0];
        v = velocities(mass_index, :,read_buffer_index) + T*a;
        p = positions(mass_index, :,read_buffer_index) + T*v;
        
        
        %Check collision with y=0
        if p(1) < 0
            p(1) = 0;
            v(1) = -v(1);
        end
        
        %Store information in backbuffer
        velocities(mass_index, :,write_buffer_index) = v;
        positions(mass_index, :,write_buffer_index) = p;
        
        %reset force
        forces(mass_index,:) = [0,0];
        
        p_last = positions(mass_index, :,read_buffer_index);
        %dtext(0.2+p_last(2),p_last(1),num2str(mass_index));
    end
        
    plot(positions(:,2,read_buffer_index), ...
            positions(:,1,read_buffer_index),'*');
    
    %axis manual;
    axis equal;
    axis([-2 30 0 20]);
    computation_time = toc;
    pause();
    pause(max(T-computation_time,0.001));
    
    %Swap buffer
    read_buffer_index = rem(read_buffer_index,2)+1;
    write_buffer_index = rem(write_buffer_index,2)+1;
    clf('reset');
    %toc;
end
