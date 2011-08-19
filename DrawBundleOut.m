function DrawBundleOut

fid = fopen('bundle_binocular.out');
fout = fopen('fbo_binocular_config.ini', 'w+');
scale = 0.005;
zNear = 0.01;
zFar  = 10;
window_width = 600;
window_height = 340;

ImDir = 'real_2_binocular/';
images = dir([ImDir '*.ppm']);

depth = 1.3;
pt = -[0, 0, 1]';

pt = pt*depth;

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

half = NumOfIm/2;

C_array  = zeros(3, NumOfIm);
baseline = zeros(1, half);


for i = 1:NumOfIm
    
    K = GetK(fid);
    R = GetR(fid);
    T = GetT(fid);
    
    % Draw camera
    C = -R\T;
    plot3(C(1), C(2), C(3), 'or');
    
    C_array(:, i) = C;
    
    if i > half
        baseline(i-half) = sqrt(sum((C_array(:, i-half) - C).^2));
%         disp(baseline(i-half));
    end
    
    if i == NumOfIm
        bl_mean = mean(baseline);
        bl_median = median(baseline);
        bl_std = std(baseline);
        
        disp(['average baseline: ' num2str(bl_mean)]);
        disp(['baseline median : ' num2str(bl_median)]);
        disp(['baseline std    : ' num2str(bl_std)]);
    end
    
    
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
        
        
        fprintf(fout, '%f ', C);
        fprintf(fout, CR);
        
        
         % Draw 3D point
        plot3(pt_3d(1), pt_3d(2), pt_3d(3), 'ob');
    end
    
    fprintf(fout, '%s', [ImDir images(i).name]);
    fprintf(fout, CR);
    
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
    fprintf(fout, CR);
    
    
%     screen = K*[R T]*[pt_3d; 1];
%     screen = screen/screen(end);
    
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