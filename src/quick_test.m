root = '/Users/jim/Documents/repos/matlab/json_examples';

d = dir(fullfile(root,'*.json'));
for i = 1:length(d)
    file_path = fullfile(d(i).folder,d(i).name);
    fprintf('%d: %s\n',i,d(i).name)
    h = tic;
    str = fileread(file_path);
    wtf1 = jsondecode(str);
    fprintf('  MATLAB: %0.2f\n',toc(h));
    clear str
    h = tic;
    wtf2 = json.load(file_path);
    fprintf('  Turtle: %0.2f\n',toc(h));
    clear wtf1
    clear wtf2
end

%{
addpath('/Users/jim/Documents/repos/matlab/turtle_json_benchmark/progs/matlab-json-master')
addpath('/Users/jim/Documents/repos/matlab/turtle_json_benchmark/progs/jsonlab-master');
addpath('/Users/jim/Documents/repos/matlab/turtle_json_benchmark/progs/c_json_io/c++_json_io');
%}