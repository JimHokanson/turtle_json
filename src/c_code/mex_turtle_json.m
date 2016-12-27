function mex_turtle_json()
%x Code to compile turtle_json
%
%   I am using GCC on both mac and windows. On windows, I'm using GCC
%   to allow pointers to labels for jump statements. On mac, I'm using
%   GCC to allow openMP. In the future compiler updates may this
%   unecessary. 


%Windows setup
%----------------------------------------------------------------------
%{
Compiling on Windows
--------------------
May require 2015b or newer, although older versions could work, 
if the TDM-GCC settings can be back-ported 
(2015b is the first Matlab release to officially support TDM-GCC)

 1) Install TDM-GCC with openmp 
    ***** Be sure to click that you want to install openmp

    http://tdm-gcc.tdragon.net/

 2) The matlab TDM-GCC entry on the file exchange might be needed, not sure
 https://www.mathworks.com/matlabcentral/fileexchange/52848-matlab-support-for-the-mingw-w64-c-c++-compiler-from-tdm-gcc
 
 3) Might need to run the following command. I place this in my 'startup.m' 
   file. Update the path as necessary. 
 setenv('MW_MINGW64_LOC','C:\TDM-GCC-64')
%} 

%Mac setup
%--------------------------------------------------------------------------
%{
Mac OS
------

 Terminal commands were roughly as follows:

 brew update
 xcode-select --install

 %I may have followed steps in this link :/
 http://stackoverflow.com/questions/24652996/homebrew-not-working-on-osx

 brew search gcc 

 //Took about 70 minutes on my laptop
 brew install homebrew/versions/gcc6 --without-multilib

 //May not be necessary, seems like the install does this ...
 brew link --overwrite --force gcc6
 brew unlink gcc6 && brew link gcc6 
 brew install --with-clang llvm

%}



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

    mex(FLAGS{:},'json_info_to_data.c');

    cd(start_path);

end