function object_tests()
%
%   json.tests.object_tests

tests(1,:) = {'{"key":1,3}',1,'3 should be a key, not a numeric'};

%TODO: Throw in tests of numerical accuracy

n_tests = size(tests,1);
for iTest = 1:n_tests
    cur_test_string = tests{iTest,1};
    expected_value = tests{iTest,2};
    should_pass = isempty(tests{iTest,3});
    try
        t = json.stringToTokens(cur_test_string);
        if ~should_pass
           error_string = sprintf('Test #%d should have thrown an error but didn''t',iTest);
           error(error_string);
        end
        fprintf('Test %d passed as expected\n',iTest);
    catch ME
        if should_pass
           error_string = sprintf('Test #%d should have not thrown an error but did',iTest); 
           error(error_string);
        end
        fprintf('Test %d failed as expected\n',iTest);
    end

end