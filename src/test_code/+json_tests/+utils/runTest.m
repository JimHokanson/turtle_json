function [output_test_number,tokens,data] = runTest(input_test_number,cur_test_string,error_id,memo,expected_value)
%
%   json_tests.utils.runTest(input_test_number,cur_test_string,error_id,memo,expected_value)
%
%   Inputs
%   ------
%   

persistent test_number

    if input_test_number == 1
        test_number = 1;
    else
        test_number = test_number + 1;
    end

    output_test_number = test_number;
    
    should_pass = isempty(error_id);

    try
        tokens = json.tokens.parse(cur_test_string);
        data = tokens.getParsedData();
        passed = true;
    catch ME
        passed = false;
        if should_pass
            disp(ME)
            error('Test #%d should have not thrown an error but did',test_number);
        elseif ~strcmp(ME.identifier,error_id)
            disp(ME)
            error('Test: %d failed, but with the incorrect error, expecting: %s',test_number,error_id);
        else
            fprintf('Test %d failed as expected\n',test_number);
        end
    end
    
    if passed && ~should_pass
        error('Test #%d should have thrown an error but didn''t',test_number);
    elseif passed
        
        if ~isempty(expected_value) && ~isequal(data,expected_value)
            error('Test #%d failed because the parsed data did not match the expected value',test_number)
        else
            if nargout
                %do nothing
            else
                fprintf('Test %d passed as expected\n',test_number);
            end
        end
    end

end