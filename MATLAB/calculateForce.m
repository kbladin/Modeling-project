function force_from_connection = calculateForce(k, b, l, p1, p2, v1,v2)

% position
delta_p = p1 - p2;

%Normalize delta_p
norm_delta_p = sqrt(sum(abs(delta_p).^2,2));

%if(norm_delta_p == 0)
 %   delta_p_hat = [0,0];
%else
    delta_p_hat = diag(1./norm_delta_p)*(delta_p);
%end

% velocities
delta_v = v1 - v2;

%Calculate force from connections
force_from_connection = diag((-k.*(norm_delta_p - l) - b.*(dot(delta_v', delta_p_hat')')))*delta_p_hat;

%forces(mass_index1,:) = forces(mass_index1,:) + force_from_connection;
%forces(mass_index2,:) = forces(mass_index2,:) - force_from_connection;

end