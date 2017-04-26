function mex_turtle_json()
%x Code to compile turtle_json
%
%   This code has been designed with OpenMP and pointers to labels. I've
%   used GCC for mac and windows.







%Can we extract this from the environment?
MAC_COMPILER_PATH = '/usr/local/Cellar/gcc6/6.1.0/bin/gcc-6';

    stack = dbstack('-completenames');

    base_path = fileparts(stack(1).file);
    
    start_path = cd;
    cd(base_path)

        FLAGS = {
        'CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx"'
        'LDFLAGS="$LDFLAGS -fopenmp"'
        'COPTIMFLAGS="-O3 -DNDEBUG"'
        '-O'; '-v'; '-largeArrayDims'
        };
    
    
if ismac()
    if ~exist(MAC_COMPILER_PATH,'dir')
       error('path to compiler on path not found') 
    end
    
    FLAGS = [FLAGS; {['CC=' MAC_COMPILER_PATH]}];     
end

    mex(FLAGS{:}, ...
    'turtle_json_mex.c', ...
    'turtle_json_main.c', ...
    'turtle_json_post_process.c', ...
    'turtle_json_mex_helpers.c', ...
    'turtle_json_pp_objects.c', ...
    'turtle_json_number_parsing.c');

    mex(FLAGS{:},'json_info_to_data.c','json_info_to_data__arrays.c','json_info_to_data__objects.c','json_info_to_data__utils.c','json_info_to_data__option_handling.c');

    cd(start_path);

end