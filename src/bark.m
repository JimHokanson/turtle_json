function varargout = bark()

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
	jt = json.tokens(file_path,'chars_per_token',5);
    d1_all(i) = jt.toc_non_read_time;
    %sobj = jt.get_parsed_data();
end
fprintf('%g, %g, %g\n',toc(t3)/20,min(d1_all),sum(d1_all)/2)

if nargout
   varargout{1} = jt; 
end

end