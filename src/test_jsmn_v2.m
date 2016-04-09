function test_jsmn_v2


t = tic;
%profile on
for i = 1:1
  
file_path = 'G:\repos\matlab_git\jsmn_mex\example_data\canada.json'; 
file_path = 'G:\repos\matlab_git\jsmn_mex\example_data\citm_catalog.json'; 

file_path = 'H:\example_data\1.json';
N = 2;
d1_all = zeros(1,N);
t3 = tic;
for i = 1:N
    %jt = json.tokens(file_path);
	jt = json.tokens(file_path,'chars_per_token',10);
    d1_all(i) = jt.d1;
    %sobj = jt.get_parsed_data();
end
fprintf('%g, %g, %g\n',toc(t3)/N,min(d1_all),sum(d1_all))


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
for i = 1:20
    %jt = json.tokens(file_path);
	jt = json.tokens(file_path,'chars_per_token',10);
    d1_all(i) = jt.d1;
    %sobj = jt.get_parsed_data();
end
fprintf('%g, %g, %g\n',toc(t3)/20,min(d1_all),sum(d1_all)/2)
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
%0.2764, 0.168005, 1.69757 //Removed c to Matlab discrepancies
%
%
%

%No math
%0.4,
%0.76, 0.54, 5.47
%0.5, 0.27, 2.82

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