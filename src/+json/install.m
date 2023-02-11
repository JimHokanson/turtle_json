function install
%
%   json.install
%
%   This needs to be called once to move the relevant binary into place.
%
%   See Also
%   --------
%   mex_turtle_json
%
%   Improvements
%   ------------
%   1. Move files bandling code somewhere that the compile step can use it

if ismac()
    [~,result] = system('uname -v');
    is_m1_mac = any(strfind(result,'ARM64'));
    
    p = mfilename('fullpath');
    
    src_path = fileparts(fileparts(p));
    
    c_root = fullfile(src_path,'c_code');
    
    r1 = 'turtle_json_mex';
    r2 = 'json_info_to_data';
    ext = '.mexmaci64';
    
    if is_m1_mac
        suffix = '_m1_rosetta';
    else
        suffix = '_intel_mac';
    end
    
    source1 = fullfile(c_root,[r1 suffix ext]);
    source2 = fullfile(c_root,[r2 suffix ext]);
    
    target1 = fullfile(c_root,[r1 ext]);
    target2 = fullfile(c_root,[r2 ext]);
    if ~exist(target1,'file')
        copyfile(source1,target1);
    end
    if ~exist(target2,'file')
        copyfile(source2,target2);
    end

    %Copy file ...
    
end

end