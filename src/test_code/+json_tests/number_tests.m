function number_tests()
%
%   json_tests.number_tests

%Column 1: json string
%Column 2: error identifier or expected value
%Column 3: Internal note as to what's happening

tests(1,:) = {['[12312312,1231,12,' ...
    '123123123123123123123123123123,123123]'],...
    'turtle_json:integer_component_too_large','Too many digits in the number'};
tests(end+1,:) = {'[1.]','turtle_json:no_fractional_numbers','Number with a period had no fractional numbers that followed'};
tests(end+1,:) = {'[+1]','turtle_json:invalid_token','Can''t lead with a positive'};

% tests(1,:) = 
% tests(2,:) = {'[+1.]',0,'''.'' needs to be followed by a numeric value'}; %
% tests(3,:) = {'[1.3]',1.3,''};
% tests(4,:) = {'[-1.3]',-1.3,''};
% tests(5,:) = {'[-1.3e1]',-1.3e1,''};
% tests(6,:) = {'[-1.3E1]',-1.3E1,''};
% tests(7,:) = {'[-1.3E]',0,'exponent needs to be followed by a numeric value'};
% tests(8,:) = {'[5E3]',5E3,''};
% tests(9,:) = {'[5E+3]',5E+3,''};
% tests(10,:) = {'[5E-3]',5E-3,''};

%TODO: Throw in tests of numerical accuracy

n_tests = size(tests,1);
for iTest = 1:n_tests
    cur_test_string = tests{iTest,1};
    expected_value_or_msg = tests{iTest,2};
    should_pass = isempty(tests{iTest,3});
    passed = true;
    try
        t = json.stringToTokens(cur_test_string);
        fprintf('Test %d passed as expected\n',iTest);
    catch ME
        passed = false;
        if should_pass
            error('Test #%d should have not thrown an error but did',iTest);
        elseif ~strcmp(ME.identifier,expected_value_or_msg)
            fprintf(2,'Expected error id:  %s\n',expected_value_or_msg);
            fprintf(2,'Observed error id:  %s\n',ME.identifier);
            error('Test #%d failed, mismatch in error IDs, failed with message: %s',iTest,ME.message);
        else
            fprintf('Test %d failed as expected with message:\n         %s\n',iTest,ME.message);
        end
    end
    if passed && ~should_pass
        error_string = sprintf('Test #%d should have thrown an error but didn''t',iTest);
        error(error_string);
    end
end