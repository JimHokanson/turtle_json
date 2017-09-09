function mex_turtle_json()
%x Code to compile turtle_json
%
%   mex_turtle_json
%
%   This code has been designed with OpenMP and pointers to labels. I've
%   used GCC for mac and windows.
%
%   TODO: Add support for detecting whether the output file is locked ...

%This file is using mex_maker:
%https://github.com/JimHokanson/mex_maker

%Compiling of turtle_json_mex.c and associated files
%-------------------------------------------------------
fprintf('Compiling turtle_json_mex.c\n');
c = mex.compilers.gcc('./turtle_json_mex.c',...
    'files',{...
    './turtle_json_main.c', ...
    './turtle_json_post_process.c', ...
    './turtle_json_mex_helpers.c', ...
    './turtle_json_pp_objects.c', ...
    './turtle_json_number_parsing.c'});
c.addLib('openmp');
c.addCompileFlags('-mavx');
c.build();

%Compiling of json_info_to_data.c and associated files
%--------------------------------------------------------
fprintf('Compiling json_info_to_data.c\n');
c = mex.compilers.gcc('./json_info_to_data.c',...
    'files',{...
    './json_info_to_data__arrays.c', ...
    './json_info_to_data__objects.c', ...
    './json_info_to_data__utils.c', ...
    './json_info_to_data__option_handling.c'});
c.build();

end