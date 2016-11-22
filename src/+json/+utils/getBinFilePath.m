function file_path = getBinFilePath(file_name)
%
%   file_path = json.utils.getBinFilePath(file_name)
%
%   Example
%   -------
%   file_path = json.utils.getBinFilePath('ut8_01.json');

    file_path = fullfile(json.utils.getBinRoot,file_name);


end