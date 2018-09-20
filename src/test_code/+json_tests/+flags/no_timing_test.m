function no_timing_test()

%TODO: Run some smaller files with and without timing enabled

%   TODO: enable modifying flags in mex_turtle_json

file_path = json.utils.examples.getFilePath('citm_catalog.json');

file_path = json.utils.examples.getFilePath('svg_menu.min.json');

file_path = json.utils.examples.getFilePath('twitter.json');


profile on

N = 1000;
tic
for i = 1:N
    data = json.load(file_path);
end
temp = 1000*toc/N;
fprintf('Average parse time %g (ms)\n',temp);

N = 1000;
tic
for i = 1:N
    %This is the most direct way of making these calls
    mex_result = turtle_json_mex(file_path);
    data = json_info_to_data(0, mex_result, 1);
end
temp = 1000*toc/N;
fprintf('Average parse time %g (ms)\n',temp);


profile off
profile viewer
 

end