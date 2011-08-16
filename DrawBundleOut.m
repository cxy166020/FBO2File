function DrawBundleOut

fid = fopen('bundle.out');

% Read comment line
fgetl(fid);

% Read number of input images
line = fgetl(fid);

c = textscan(line, '%d');

NumOfIm = c{1}(1);

figure, hold on

for i = 1:NumOfIm
    
    K = GetK(fid);
    R = GetR(fid);
    T = GetT(fid);
    
    % Draw camera
    C = -R\T;
    plot3(C(1), C(2), C(3), 'or');
    
    % Draw principal axis
    M = K*R;
    pa = det(M)*M(3, :);
    pa = pa/norm(pa);
    
    plot3([C(1), C(1)+pa(1)], [C(2), C(2)+pa(2)], [C(3), C(3)+pa(3)], '-g');
end

hold off

fclose(fid);


function K = GetK(fid)

line = fgetl(fid);
c = textscan(line, '%f');
f = c{1}(1);

K = eye(3);
K(1, 1) = f;
K(2, 2) = f;


function R = GetR(fid)

R = zeros(3, 3);

line = fgetl(fid);
c = textscan(line, '%f');
R(1, :) = c{1}';

line = fgetl(fid);
c = textscan(line, '%f');
R(2, :) = c{1}';

line = fgetl(fid);
c = textscan(line, '%f');
R(3, :) = c{1}';


function T = GetT(fid)

line = fgetl(fid);
c = textscan(line, '%f');
T = double(c{1});