Compilation Notes
-----------------

I have compiled mex files for Windows and Mac OS using:
TODO: Fill this in

If you wish to compile from source, follow the following instructions:

Compiling on Windows
--------------------
May require 2015b or newer, although older versions could work, if the TDM-GCC settings can be back-ported (2015b is the first Matlab release to officially support TDM-GCC)

 1) Install TDM-GCC with openmp (Be sure to click that you want to install openmp)
 http://tdm-gcc.tdragon.net/
 2) The matlab TDM-GCC entry on the file exchange might be needed, not sure
 https://www.mathworks.com/matlabcentral/fileexchange/52848-matlab-support-for-the-mingw-w64-c-c++-compiler-from-tdm-gcc
 3) Might need to run:  
 setenv('MW_MINGW64_LOC','C:\TDM-GCC-64')
 4) Make the mex call
 mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" turtle_json_mex.c turtle_json_main.c turtle_json_post_process.c -O -v
 
Mac OS
------
TODO: Clean this up ...

 //This required installing gcc ..., couldn't get openmp working with xcode
 mex CC='/usr/local/Cellar/gcc6/6.1.0/bin/gcc-6' CFLAGS="$CFLAGS -std=c11 -fopenmp -mavx" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" turtle_json_mex.c turtle_json_main.c turtle_json_post_process.c -O -v

 *
 brew update
 xcode-select --install
 http://stackoverflow.com/questions/24652996/homebrew-not-working-on-osx
 

 brew search gcc 
 //Took about 70 minutes on my laptop
 brew install homebrew/versions/gcc6 --without-multilib
 //May not be necessary, seems like the install does this ...
 brew link --overwrite --force gcc6
 brew unlink gcc6 && brew link gcc6 
 brew install --with-clang llvm
 
 *