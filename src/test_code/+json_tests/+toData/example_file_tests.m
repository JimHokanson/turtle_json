function example_file_tests()
%
%   json_tests.toData.example_file_tests

file_list = json.utils.examples.FILE_LIST;
for iFile = 1:length(file_list)
    try
        data = json_tests.utils.time_example_file(iFile,'n_runs',1);
    catch ME
        disp(ME)
        error('Parsing example file failed: \n%s\n',file_list{iFile});
    end
    fprintf('Parsing example file #%d passed\n',iFile);
end

end