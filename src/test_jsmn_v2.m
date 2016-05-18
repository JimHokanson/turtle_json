function test_jsmn_v2

%setenv('MW_MINGW64_LOC','C:\TDM-GCC-64')

t = tic;
%profile on
for i = 1:1
  
file_path = 'G:\repos\matlab_git\jsmn_mex\example_data\canada.json'; 
file_path = 'G:\repos\matlab_git\jsmn_mex\example_data\citm_catalog.json'; 

file_path = 'H:\example_data\1.json';
N = 5;
d1_all = zeros(1,N);
t3 = tic;
for i = 1:N
    jt = json.tokens(file_path);
	%jt = json.tokens(file_path,'chars_per_token',10);
    d1_all(i) = jt.toc_non_read_time;
    %sobj = jt.get_parsed_data();
end
fprintf('%g, %g, %g\n',toc(t3)/N,min(d1_all),sum(d1_all))
%3.92549, 3.39944, 6.8132
%3.58606, 3.07785, 6.25688 - removed starts and ends
%3.54081, 2.68656, 5.47246 - no math
%1.14952, 0.474847, 0.966987 - no strings
%0.780231, 0.482823, 0.993973
%
%
%
%0.603803, 0.359887, 0.72572
%
%0.560783, 0.306171, 1.54996    5x - bumped up from running 2x
%0.481109, 0.228823, 1.17341  - using GOTO
%0.468115, 0.233291, 1.18439 - ""
%0.42677, 0.197664, 0.993281

%Current times from master
%-------------------------
%3.44778, 2.95078, 5.92744 - string and # processing

%0.544709, 0.276992, 1.41813

tic; 
for i = 1:10
wtf = read_to_uint8(file_path); 
end
toc;

tic; 
for i = 1:10
wtf = sl.io.fileRead(file_path,'*uint8');
end
toc;

clear all
p1 = 'G:\repos\matlab_git\jsmn_mex\vs_project\jsmn_mex\x64\Release\jsmn_mex.mexw64';
p2 = 'G:\repos\matlab_git\jsmn_mex\src\jsmn_mex.mexw64';
copyfile(p1,p2);

cd G:\repos\matlab_git\jsmn_mex\src
setenv('MW_MINGW64_LOC','C:\TDM-GCC-64')
mex CFLAGS="$CFLAGS -std=c11 -fopenmp -mtune=ivybridge -msse4.2" LDFLAGS="$LDFLAGS -fopenmp" COPTIMFLAGS="-O3 -DNDEBUG" jsmn_mex.c jsmn.c jsmn_mex_post_process.c -O 

if ismac
file_root = '/Users/jameshokanson/Google Drive/OpenWorm/OpenWorm Public/Movement Analysis/example_data/WCON';
else
file_root = 'C:\Users\RNEL\Google Drive\OpenWorm\OpenWorm Public\Movement Analysis\example_data\WCON\';
end
file_name = 'testfile_new.wcon';
file_name = 'XJ30_NaCl500mM4uL6h_10m45x10s40s_Ea.wcon';
file_path = fullfile(file_root,file_name);
d1_all = zeros(1,20);
t3 = tic;
for i = 1:1
    %jt = json.tokens(file_path);
	jt = json.tokens(file_path,'chars_per_token',5);
    d1_all(i) = jt.toc_non_read_time;
    %sobj = jt.get_parsed_data();
end
fprintf('%g, %g, %g\n',toc(t3)/20,min(d1_all),sum(d1_all)/2)




profile on
N = 2;
t3 = tic;
for i = 1:N
% % %     %jt = json.tokens(file_path);
% % % 	jt = json.tokens(file_path,'chars_per_token',5);
% % %     d1_all(i) = jt.d1;
    sobj = jt.get_parsed_data();
end
fprintf('%g\n',toc(t3)/N)
profile off
profile viewer



%0.48724, 0.275296, 2.8141
%0.492493, 0.277961, 2.8222
%0.483474, 0.264727, 2.71777
%0.480431, 0.26557, 2.68592
%0.46748, 0.261154, 2.64825
%0.426512, 0.213866, 2.18608
%0.431448, 0.216494, 2.21083
%0.429771, 0.215826, 2.1721
%0.425321, 0.209873, 2.12384
%
%0.390577, 0.180577, 1.817
%0.386232, 0.179044, 1.8223 //Nice speedup on not messing as much with the numeric pointer
%
%0.359798, 0.15692, 1.58492 //Complete state machine
%0.375252, 0.16457, 1.66198
%
%0.442107, 0.155828, 1.57115 //Fixed a bug
%
%0.2764, 0.168005, 1.69757 //Removed c to Matlab discrepancies (BIG FIX)
%
%0.2745, 0.127264, 1.29634 - no number resolving
%
%0.293459, 0.164545, 1.67481 - strings resolved in the function, math is back
%
%0.191358, 0.102472, 1.03972 - goto - no string or num processing
%
%0.177615, 0.0883615, 0.902094 - first round of computed goto optimizations

%Current times from master
%-------------------------
%3 different runs
% 0.302934, 0.172757, 1.74443
% 0.32871, 0.164724, 1.69345
% 0.297383, 0.167984, 1.70325

%New version - goto refactoring
%-------------------------------
%   0.263153, 0.209014, 2.12919

%0.258993, 0.205119, 2.0911 - swith instead of goto
%0.252532, 0.199187, 2.0213 - goto instead of switch
%0.272186, 0.215841, 2.21788 - with TDM-GCC - switch
%0.272117, 0.218557, 2.2447 - "  " goto


%0.18034, 0.130137, 1.34816 - no post processing TDM-GCC
%0.180698, 0.129493, 1.36089 - no nthings
%0.182756, 0.1321, 1.37136 - no TAC storage
%0.174785, 0.125994, 1.28713 - using GOTO again

%OLD NUMBERS
%============================
%2.58 - 2016-03-21, removed test for null tokens
%2.50 - 2016-03-22, removed error checking at the end for closed arrays and objects
%- minor refactoring of code
%2.26 - 2016-03-22, removed double string parsing, only parsing once
%2.25 - 2016-03-25, removed unecessary loop checks and moved token
%allocation to main loop
%2.18 - 2016-03-25, time now includes read time and token fixing time, 
%saved a LARGE amount of time by reading into uint8 and avoiding
%mxArrayToString
%0.94 - 2016-03-27, some reorganization and custom number parser
%0.93 - 2016-03-27, brought out token initialization, created super_token_is_string
%variable
%0.86 - 2016-03-27, large initial allocation to avoid reallocating ...
%0.81 - 2016-03-28, doubles are separate
%0.79 - 2016-03-29, parser->pos to parser_position variable
%0.77 - 2016-03-29 - changed numeric parser

%0.4096 - no number math

jt = json.tokens(file_path);
root = jt.get_root_token;


%i.e. the code isn't doing -1
t2 = tic;
for i = 1:10
obj = parse_json(jt.file_string,jt.info,jt.numeric_data);
end
toc(t2)/10
%0.64 - 2016-03-21
end
%profile off
toc(t);

h = load('wtf2.mat');
isequal(h.wtf2,token_info)



end