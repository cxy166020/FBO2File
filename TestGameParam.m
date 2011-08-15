clear
clc

close all

K = [633.8 0 313.8; 0 644.3 153.9; 0 0 1 ];

R = [7.0099997093e-01 5.1791569502e-01 -4.9026765507e-01;
-5.4920682275e-01 8.3058956479e-01 9.2156609709e-02;
4.5494055282e-01 2.0465656041e-01 8.6668609409e-01];

T = [4.5494055282e-01 2.0465656041e-01 8.6668609409e-01]';


C = -R\T;

pt = [240*540 320*540 540]';
pt_3d = R\(K\pt) - R\T;

M = K*R;
% Note that pricipal axes is flipped
pa = det(M)*M(3, :);

pa = pa/2500;

%test = K*[R T]*[pt_3d; 1];
%test = test/(test(end))


up = R\[0 1 0]';

up = up*50;



% Plot the whole thing 

% plot camera
plot3(C(1), C(2), C(3), '-or' , 'LineWidth', 3)
hold on

% Plot principle axis
plot3([C(1) pa(1)+C(1)], [C(2) pa(2)+C(2)], [C(3), pa(3)+C(3)], ...
    'y' , 'LineWidth', 4')

plot3( [C(1) pt_3d(1)], [C(2) pt_3d(2)], [C(3) pt_3d(3)], '-r', 'LineWidth', 1);


% plot up vector
plot3( [C(1) C(1)+up(1)], [C(2) C(2)+up(2)], [C(3) C(3)+up(3)],...
    '-g', 'LineWidth', 2 )

% plot up vector 
plot3( [pt_3d(1) pt_3d(1)+up(1)], [pt_3d(2) pt_3d(2)+up(2)],...
        [pt_3d(3) pt_3d(3)+up(3)], '-g', 'LineWidth', 2 )
 
 hold off