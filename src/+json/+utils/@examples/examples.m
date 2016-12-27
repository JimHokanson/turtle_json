classdef examples
    %
    %   Class:
    %   json.utils.examples
    
    %TODO: Move all of the example path manipulation into here
    
    properties (Constant)
        FILE_LIST = {
            'big.json'};
        DRIVE_PATH = 'https://drive.google.com/drive/folders/0B7to9gBdZEyGMExwTFA0ZWh1OTA?usp=sharing';
        
        %Apparently we can't download directly from Google drive anymore :/
        %https://gsuiteupdates.googleblog.com/2015/08/deprecating-web-hosting-support-in.html
        %http://stackoverflow.com/questions/25010369/wget-curl-large-file-from-google-drive
    end
    
    methods (Static)
        function getFilePath(file_name_or_index,ensure_exists)
            
        end
        function bin_path = getBinRoot()
            %
            %   json.utils.examples.getBinRoot()
            
            root_path = fileparts(json.sl.stack.getPackageRoot());

            bin_path = fullfile(root_path,'bin'); 
        end
    end
    
end

