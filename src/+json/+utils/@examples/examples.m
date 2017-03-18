classdef examples
    %
    %   Class:
    %   json.utils.examples

    %TODO: Get the 1.json file into here
    properties (Constant)
        FILE_LIST = {
            '1.json'        
            'big.json'          
            'canada.json'       
            'citm_catalog.json' 
            'citylots.json'      
            'large-dict.json'   
            'medium-dict.json' 
            'small-dict.json' 
            'twitter.json' 
            'utf8_01.json' 
            'wcon_testfile_new.json' 
            'XJ30_NaCl500mM4uL6h_10m45x10s40s_Ea.json'
            };
        DRIVE_PATH = 'https://drive.google.com/drive/folders/0B7to9gBdZEyGMExwTFA0ZWh1OTA?usp=sharing';
        
        %Apparently we can't download directly from Google drive anymore :/
        %
        %   Actually, this may be possible, with a bit of work ...
        %
        %https://gsuiteupdates.googleblog.com/2015/08/deprecating-web-hosting-support-in.html
        %http://stackoverflow.com/questions/25010369/wget-curl-large-file-from-google-drive
    end
    
    methods (Static)
        function file_path = getFilePath(file_name_or_index)
            %
            %   file_path = json.utils.examples.getFilePath(file_name_or_index)
            %
            %   TODO: allow regex if strcmp fails
            
            root_path = json.utils.examples.getExamplesRoot();
            if ischar(file_name_or_index)
                file_name = file_name_or_index;
            else
                file_name = json.utils.examples.FILE_LIST{file_name_or_index};
            end
            file_path = fullfile(root_path,file_name);
        end
        function bin_path = getExamplesRoot()
            %
            %   json.utils.examples.getExamplesRoot()
            
            root_path = fileparts(json.sl.stack.getPackageRoot());

            bin_path = fullfile(root_path,'examples'); 
        end
%         function downloadFile(file_name)
%             
%         end
    end
    
end

