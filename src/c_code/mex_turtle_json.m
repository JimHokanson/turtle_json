function mex_turtle_json(file_id)
%x Code to compile turtle_json
%
%   mex_turtle_json
%
%   This code has been designed with OpenMP and pointers to labels. I've
%   used GCC for mac and windows.
%
%   TODO: Document file_id
%   TODO: Add verbose option
%
%   TODO: move clearing code to mex maker

if nargin == 0
   file_id = [];
end

%This file is using mex_maker:
%https://github.com/JimHokanson/mex_maker

%Compiling of turtle_json_mex.c and associated files
%-------------------------------------------------------
if isempty(file_id) || file_id == 1
fprintf('Compiling turtle_json_mex.c\n');

%TODO: mex maker should do this ...
clear turtle_json_mex
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
end

%Compiling of json_info_to_data.c and associated files
%--------------------------------------------------------
if isempty(file_id) || file_id == 2
fprintf('Compiling json_info_to_data.c\n');
clear json_info_to_data
c = mex.compilers.gcc('./json_info_to_data.c',...
    'files',{...
    './json_info_to_data__arrays.c', ...
    './json_info_to_data__objects.c', ...
    './json_info_to_data__utils.c', ...
    './json_info_to_data__option_handling.c'});
c.build();
end

end
