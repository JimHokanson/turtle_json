function install
%
%   json.install
%
%   for macs, copt relevant binary ...

if ismac()
    [~,result] = system('uname -v');
    is_m1_mac = any(strfind(result,'ARM64'));
    
    p = mfilename('fullpath');
    
    keyboard
    
    %Copy file ...
    
end

end