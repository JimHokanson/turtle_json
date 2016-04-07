function test_all_files()

files = cell(1,5);
files{1} = 'H:\example_data\canada.json'; 
files{2} = 'H:\example_data\citm_catalog.json'; 
files{3} = 'H:\example_data\1.json'; 
files{4} = 'C:\Users\RNEL\Google Drive\OpenWorm\OpenWorm Public\Movement Analysis\example_data\WCON\testfile_new.wcon';
files{5} = 'C:\Users\RNEL\Google Drive\OpenWorm\OpenWorm Public\Movement Analysis\example_data\WCON\XJ30_NaCl500mM4uL6h_10m45x10s40s_Ea.wcon';

% iFile = 5;
% chars_per_token = 12;
% cur_file = files{5};


tic
for chars_per_token = 2:2:20
    for iFile = 1:5
        cur_file = files{iFile};
        fprintf('File %d , chars %d\n',iFile,chars_per_token);
        jt = json.tokens(cur_file,'chars_per_token',chars_per_token);
    end 
end
toc
%12.102
%11.91 - removed tokens in parse_string
%11.56 - starts is its own thing now ...
