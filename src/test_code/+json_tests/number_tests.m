function number_tests()
%
%   json.tests.number_tests

tests(1,:) = {'[+1]',1,'Can''t lead with a positive'};
tests(2,:) = {'[+1.]',0,'''.'' needs to be followed by a numeric value'}; %
tests(3,:) = {'[1.3]',1.3,''};
tests(4,:) = {'[-1.3]',-1.3,''};
tests(5,:) = {'[-1.3e1]',-1.3e1,''};
tests(6,:) = {'[-1.3E1]',-1.3E1,''};
tests(7,:) = {'[-1.3E]',0,'exponent needs to be followed by a numeric value'};
tests(8,:) = {'[5E3]',5E3,''};
tests(9,:) = {'[5E+3]',5E+3,''};
tests(10,:) = {'[5E-3]',5E-3,''};

%TODO: Throw in tests of numerical accuracy

n_tests = size(tests,1);
for iTest = 1:n_tests
    cur_test_string = tests{iTest,1};
    expected_value = tests{iTest,2};
    should_pass = isempty(tests{iTest,3});
    passed = true;
    try
        t = json.stringToTokens(cur_test_string);
        fprintf('Test %d passed as expected\n',iTest);
    catch ME
        passed = false;
        if should_pass
            error_string = sprintf('Test #%d should have not thrown an error but did',iTest);
            error(error_string);
        end
        fprintf('Test %d failed as expected with message:\n         %s\n',iTest,ME.message);
    end
    if passed && ~should_pass
        error_string = sprintf('Test #%d should have thrown an error but didn''t',iTest);
        error(error_string);
    end
end