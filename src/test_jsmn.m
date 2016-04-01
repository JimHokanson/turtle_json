function test_jsmn


t = tic;
%profile on
for i = 1:1
    
file_path = 'C:\Users\RNEL\Google Drive\OpenWorm\OpenWorm Public\Movement Analysis\example_data\WCON\XJ30_NaCl500mM4uL6h_10m45x10s40s_Ea.wcon';
t1 = tic;
for i = 1:10
str = fileread(file_path);
end
toc(t1)/10
%0.26s - reading from file

%0.34s - converting from string to char
t2 = tic;
for i = 1:10
str = sl.io.fileRead(file_path,'*uint8');
end
toc(t2)/10
%0.07s - 

t3 = tic;
for i = 1:10
[token_info,numeric_data] = getJsonTokenInfo(str);
end
toc(t3)/10
%2.58 - 2016-03-21, removed test for null tokens
%2.50 - 2016-03-22, removed error checking at the end for closed arrays and objects
%- minor refactoring of code
%2.26 - 2016-03-22, removed double string parsing, only parsing once
%2.25 - 2016-03-25, removed unecessary loop checks and moved token
%allocation to main loop

%i.e. the code isn't doing -1
t2 = tic;
for i = 1:10
obj = parse_json(str,token_info,numeric_data);
end
toc(t2)/10
%0.64 - 2016-03-21
end
%profile off
toc(t);

h = load('wtf2.mat');
isequal(h.wtf2,token_info)



end