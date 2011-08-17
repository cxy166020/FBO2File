function DrawBundleOut

fid = fopen('bundle.out');
fout = fopen('fbo_config.ini', 'w+');
scale = 3;
zNear = 550;
zFar  = 1200;
window_width = 600;
window_height = 340;

pt = -[0*650 0*650 650]';

CR = '\n';

% Read comment line
fgetl(fid);

% Read number of input images
line = fgetl(fid);

c = textscan(line, '%d');

NumOfIm = c{1}(1);

figure, hold on


fprintf(fout, '%d', NumOfIm);
fprintf(fout, CR);

fprintf(fout, '%f', scale);
fprintf(fout, CR);

fprintf(fout, '%f', zNear);
fprintf(fout, CR);

fprintf(fout, '%f', zFar);
fprintf(fout, CR);

fprintf(fout, '%d', window_width);
fprintf(fout, CR);

fprintf(fout, '%d', window_height);
fprintf(fout, CR);

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
    
    plot3([C(1), -pa(1)+C(1)], [C(2), -pa(2)+C(2)], [C(3), -pa(3)+C(3)], '-g');
    up = R\[0 1 0]';
    
    
    if i == 1
        
        % Terrain vector
        fprintf(fout, '%f ', up);
        fprintf(fout, CR);
        
        % 3D point location
        pt_3d = R\(K\pt) - R\T;
        fprintf(fout, '%f ', pt_3d);
        fprintf(fout, CR);
        
        % Camera location of view 1
        C = -R\T;
        fprintf(fout, '%f ', C);
        fprintf(fout, CR);
        
    end
    
    % Up vector
    fprintf(fout, '%f ', up);
    fprintf(fout, CR);
    
    % Focal length
    fprintf(fout, '%f', K(1));
    fprintf(fout, CR);
    
    % Rotation
    fprintf(fout, '%f ', R');
    fprintf(fout, CR);
    
    % Translation
    fprintf(fout, '%f ', T);
    fprintf(fout, CR)
    
%     screen = K*[R T]*[pt_3d; 1];
%     screen = screen/screen(end)
    
end

hold off

fclose(fid);
fclose(fout);


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