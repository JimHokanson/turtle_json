function mex_turtle_json()
%
%   Code to compile turtle_json

mac_compiler_path = '/usr/local/Cellar/gcc6/6.1.0/bin/gcc-6';

    stack = dbstack('-completenames');

    base_path = fileparts(stack(1).file);
    
    start_path = cd;
    cd(base_path)

if ispc()
  mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" ...
    COPTIMFLAGS="-O3 -DNDEBUG" ...
    turtle_json_mex.c ...
    turtle_json_main.c ...
    turtle_json_post_process.c ...
    turtle_json_mex_helpers.c ...
    turtle_json_pp_objects.c ...
    -O -v -largeArrayDims


    mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" ...
        COPTIMFLAGS="-O3 -DNDEBUG" json_info_to_data.c

else
    
    FLAGS = {['CC=' mac_compiler_path]
        'CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx"'
        'LDFLAGS="$LDFLAGS -fopenmp"'
        'COPTIMFLAGS="-O3 -DNDEBUG"'
        '-O'; '-v'; '-largeArrayDims'
        };
    
    mex(FLAGS{:}, ...
    'turtle_json_mex.c','turtle_json_main.c','turtle_json_post_process.c',...
    'turtle_json_mex_helpers.c','turtle_json_pp_objects.c');
    mex(FLAGS{:},'json_info_to_data.c');
        
end
    
    cd(start_path);

end