function mex_turtle_json()

if ispc()
  mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" ...
    COPTIMFLAGS="-O3 -DNDEBUG" turtle_json_mex.c turtle_json_main.c ...
    turtle_json_post_process.c turtle_json_mex_helpers.c -O -v -largeArrayDims


    mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" ...
        COPTIMFLAGS="-O3 -DNDEBUG" json_info_to_data.c

else
    mex CC='/usr/local/Cellar/gcc6/6.1.0/bin/gcc-6'  ...
        CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" ...
        COPTIMFLAGS="-O3 -DNDEBUG" json_info_to_data.c
    
end
    
    

end