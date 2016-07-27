function bin_path = getBinRoot()
%
%   bin_path = json.utils.getBinRoot()

root_path = fileparts(json.sl.stack.getPackageRoot());

bin_path = fullfile(root_path,'bin');

end