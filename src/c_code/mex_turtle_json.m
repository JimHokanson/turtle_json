function mex_turtle_json()

  mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" ...
COPTIMFLAGS="-O3 -DNDEBUG" turtle_json_mex.c turtle_json_main.c ...
  turtle_json_post_process.c turtle_json_mex_helpers.c -O -v -largeArrayDims

end