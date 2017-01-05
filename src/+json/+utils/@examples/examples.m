classdef examples
    %
    %   Class:
    %   json.utils.examples

    %TODO: Get the 1.json file into here
    properties (Constant)
        FILE_LIST = {
            'big.json' %1
            'canada.json' %2
            'citm_catalog.json' %3
            'large-dict.json' %4
            'medium-dict.json' %5
            'small-dict.json' %6
            'twitter.json' %7
            'utf8_01.json' %8
            'wcon_testfile_new.json' %9
            'XJ30_NaCl500mM4uL6h_10m45x10s40s_Ea.json' %10
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
            %   json.utils.examples.getFilePath()
            
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

