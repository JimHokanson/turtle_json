cd /d G:\repos\matlab_git\jsmn_mex\src
gcc -c -pg -DMX_COMPAT_32   -DMATLAB_MEX_FILE  -I"H:\ProgramFiles\Matlab/extern/include" -I"H:\ProgramFiles\Matlab/simulink/include" -I"H:\ProgramFiles\Matlab/extern\lib\win64\mingw64" -fexceptions -fno-omit-frame-pointer -std=c11 -fopenmp -mavx -O3 -DNDEBUG G:\repos\matlab_git\jsmn_mex\src\jsmn_mex.c -o jsmn_mex.obj
gcc -c -pg -DMX_COMPAT_32   -DMATLAB_MEX_FILE  -I"H:\ProgramFiles\Matlab/extern/include" -I"H:\ProgramFiles\Matlab/simulink/include" -I"H:\ProgramFiles\Matlab/extern\lib\win64\mingw64" -fexceptions -fno-omit-frame-pointer -std=c11 -fopenmp -mavx -O3 -DNDEBUG G:\repos\matlab_git\jsmn_mex\src\jsmn.c -o jsmn.obj
gcc -c -pg -DMX_COMPAT_32   -DMATLAB_MEX_FILE  -I"H:\ProgramFiles\Matlab/extern/include" -I"H:\ProgramFiles\Matlab/simulink/include" -I"H:\ProgramFiles\Matlab/extern\lib\win64\mingw64" -fexceptions -fno-omit-frame-pointer -std=c11 -fopenmp -mavx -O3 -DNDEBUG G:\repos\matlab_git\jsmn_mex\src\jsmn_mex_post_process.c -o jsmn_mex_post_process.obj
gcc -m64 -pg -Wl,--no-undefined -fopenmp -shared -s -Wl,jsmn_mex.obj jsmn.obj jsmn_mex_post_process.obj   -L"H:\ProgramFiles\Matlab\extern\lib\win64\mingw64" -llibmx -llibmex -llibmat -lm -llibmwlapack -llibmwblas -o wtf.exe

%removeed shared
gcc -m64 -pg -Wl,--no-undefined -fopenmp  -s -Wl,jsmn_mex.obj jsmn.obj jsmn_mex_post_process.obj   -L"H:\ProgramFiles\Matlab\extern\lib\win64\mingw64" -llibmx -llibmex -llibmat -lm -llibmwlapack -llibmwblas -o wtf.exe

%This worked, now missing libmex.dll

%Added H:\ProgramFiles\Matlab\bin\win64 to path

%Program runs but crashes