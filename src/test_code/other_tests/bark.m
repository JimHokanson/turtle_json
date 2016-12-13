function varargout = bark(N)

if nargin == 0
   N = 20; 
end

if ismac
file_root = '/Users/jameshokanson/Google Drive/OpenWorm/OpenWorm Public/Movement Analysis/example_data/WCON';
else
file_root = 'C:\Users\RNEL\Google Drive\OpenWorm\OpenWorm Public\Movement Analysis\example_data\WCON\';
if ~exist(file_root)
    file_root = 'C:\D\data';
end
end
file_name = 'testfile_new.wcon';
file_name = 'XJ30_NaCl500mM4uL6h_10m45x10s40s_Ea.wcon';
file_path = fullfile(file_root,file_name);
d1_all = zeros(1,N);
t3 = tic;
for i = 1:N
    %jt = json.tokens(file_path);
	jt = json.tokens(file_path,'chars_per_token',5);
    d1_all(i) = jt.parse_info.timing_info.non_read_time;
    %sobj = jt.get_parsed_data();
end
fprintf('avg total, min parse, avg parse, sum 10 parses\n');
fprintf('%0.3g,     %0.3g,     %0.3g,     %0.3g\n',toc(t3)/N,min(d1_all),mean(d1_all),sum(d1_all)/2)

if nargout
   varargout{1} = jt; 
end

end