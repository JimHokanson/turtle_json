function file_path = getBinFilePath(file_name, ensure_exists)
%
%   file_path = json.utils.getBinFilePath(file_name)
%
%   Example
%   -------
%   file_path = json.utils.getBinFilePath('ut8_01.json');

if nargin == 1
    ensure_exists = false;
end

    file_path = fullfile(json.utils.getBinRoot,file_name);


end