%Define constants
m1 = 0.1;
m2 = 0.1;
k = 10;
b = 1;

%Modelling as State Space
A = [-b/m1  b/m1 -1/m1;
      b/m2 -b/m2  1/m2;
      k    -k      0   ];
B = [1/m1 0 0]';
C = [1 0 0;
     0 1 0;
     0 0 1];

%Simulate
sys = ss(A,B,C,0);
[y, T] = impulse(sys);
iy = cumsum(y);

dt = (T(length(T)) - T(1))/length(T);

%% Animate
close all;
hold on;
plot(T(1), y(1,:),'*');
legend('x1 = v1','x2 = v2','x3 = fjäderkraft');
for i=2:length(iy)
    plot(T(i), y(i,:),'*');
    %axis([0 2 0 max(iy(:))]);
    pause(dt);
end
