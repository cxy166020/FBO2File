clear
clc
close all

% 6.1752144734e+02 3.0765054099e-01 6.5437810008e-01
% 7.0099997093e-01 5.1791569502e-01 -4.9026765507e-01
% -5.4920682275e-01 8.3058956479e-01 9.2156609709e-02
% 4.5494055282e-01 2.0465656041e-01 8.6668609409e-01
% -1.4451260334e+00 1.5735374912e-01 -1.3348769041e-01

K = [633.8 0 0; 0 633.8 0; 0 0 1 ];

R = [7.0099997093e-01 5.1791569502e-01 -4.9026765507e-01;
    -5.4920682275e-01 8.3058956479e-01 9.2156609709e-02;
    4.5494055282e-01 2.0465656041e-01 8.6668609409e-01];

T = [-1.4451    0.1574   -0.1335]';


C = -R\T;

pt = -[0*15 0*15 15]';
pt_3d = R\(K\pt) - R\T;

M = K*R;
% Note that pricipal axes is flipped
pa = det(M)*M(3, :);

pa = pa/2500;

up = R\[0 1 0]';
%up = R*[0 1 0]';
up = up*50;

test = K*[R T]*[pt_3d; 1];
test = test/(test(end))

test1 = K*[R T]*[pt_3d+up; 1];
test1 = test1/test1(end)

y_world = up';
z_world = -pa;

x_world = cross(y_world, z_world);
z_world = cross(x_world, y_world);

x_world = x_world/norm(x_world);
y_world = y_world/norm(y_world);
z_world = z_world/norm(z_world);

x_model = [1 0 0];
y_model = [0 1 0];
z_model = [0 0 1];


rotation = [x_world' y_world' z_world'];

% Plot the whole thing 

% plot camera
plot3(C(1), C(2), C(3), '-or' , 'LineWidth', 3)
hold on

% Plot principle axis
plot3([C(1) pa(1)+C(1)], [C(2) pa(2)+C(2)], [C(3), pa(3)+C(3)], ...
    'b' , 'LineWidth', 4')

plot3( [C(1) pt_3d(1)], [C(2) pt_3d(2)], [C(3) pt_3d(3)], '-r', 'LineWidth', 1);


% plot up vector
plot3( [C(1) C(1)+up(1)], [C(2) C(2)+up(2)], [C(3) C(3)+up(3)],...
    '-g', 'LineWidth', 2 )

% plot up vector 
plot3( [pt_3d(1) pt_3d(1)+up(1)], [pt_3d(2) pt_3d(2)+up(2)],...
        [pt_3d(3) pt_3d(3)+up(3)], '-g', 'LineWidth', 2 )
    
plot3( [pt_3d(1) pt_3d(1)+x_world(1)], [pt_3d(2) pt_3d(2)+x_world(2)],...
        [pt_3d(3) pt_3d(3)+x_world(3)], '-k', 'LineWidth', 1 );
    
plot3( [pt_3d(1) pt_3d(1)+y_world(1)], [pt_3d(2) pt_3d(2)+y_world(2)],...
        [pt_3d(3) pt_3d(3)+y_world(3)], '-m', 'LineWidth', 1 )
      
plot3( [pt_3d(1) pt_3d(1)+z_world(1)], [pt_3d(2) pt_3d(2)+z_world(2)],...
        [pt_3d(3) pt_3d(3)+z_world(3)], '-c', 'LineWidth', 1 )
 
 hold off
 
 
 
 model_view = [-0.701001 -27.4603 -0.45494 245.067 
-0.517915 41.5295 -0.204657 109.932 
0.490268 4.6078 -0.866686 467.464 ];


model_view*[1 0 0 1]'
        