function test_parser

%   json.test_parser

% fail_strings = cell(1,30);
% fail_strings{1} = '"A JSON payload should be an object or array, not a string."'
% 
% 
% all_strings = {'

%{

str = '{"Missing colon" null}';
jt = json.tokens('','raw_string',str);


%}

repo_root = fileparts(sl.stack.getPackageRoot);

test_path = fullfile(repo_root,'test_files');

test_file_paths = sl.dir.getList(test_path,'output_type','paths','extension','.json');

for iFile = 1:length(test_file_paths)
   cur_file_path = test_file_paths{iFile};
   [~,file_name] = fileparts(cur_file_path);
   should_pass = isempty(strfind(file_name,'fail')); 
   fprintf('Current file: %s\n', cur_file_path);
   try
      jt = json.tokens(cur_file_path); 
      if ~should_pass
         error('Invalid file passed'); 
      end
   catch
      if should_pass
         error('Valid file failed to pass') 
      end
   end
end


end