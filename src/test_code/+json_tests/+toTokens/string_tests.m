function string_tests()
%
%   json_tests.toTokens.string_tests
%
%   See Also
%   --------
%   json_tests.json_checker_tests

%   Format
%   ------
%   1) string
%   2) empty string to pass,
%   3) notes on reason for error or thing being tested
%   4) expected answer

fh = @(x) char(org.apache.commons.lang.StringEscapeUtils.unescapeJava(x));

%Tests focused on string termination
%-----------------------------------
tests(1,:)     = {'["This is a test]',      'turtle_json:unterminated_string',  'Missing closing quotes',''};
tests(end+1,:) = {'["This is a test\"]',    'turtle_json:unterminated_string',  'Missing closing quote, quote character escaped',''};
tests(end+1,:) = {'["Hello \" World"]',     '',                                 'Escaped quote character with proper closing of string',''};
tests(end+1,:) = {'["Hello World\\"]',      '',                                 'Escape character is escaped, so string is terminated',''};
tests(end+1,:) = {'["Hello World\\\"]',     'turtle_json:unterminated_string',  'unterminated string',''};
tests(end+1,:) = {'["Hello World\\\\"]',    '',                                 'terminated string',''};
%Tests focused on the proper escapes of characters
%--------------------------------------------------
%1) Valid escape characters
%2) Characters that need to be escaped => less than 32
tests(end+1,:) = {'["This \" is a test"]','', 'Escape of " character',{sprintf('This \" is a test')}};
tests(end+1,:) = {'["This \n is a test"]','', 'Escape of \n character',{sprintf('This \n is a test')}};
tests(end+1,:) = {'["15\u00f8C 3\u0111"]','', 'Escape of unicode characters',{fh('15\u00f8C 3\u0111')}};

%TODO: Add on UTF-8 check - need sprintf



n_tests = size(tests,1);
for iTest = 1:n_tests
    cur_test_string = tests{iTest,1};
    error_id = tests{iTest,2};
    should_pass = isempty(error_id);
    passed = true;
    try
        t = json.parse(cur_test_string);
    catch ME
        passed = false;
        if should_pass
            disp(ME)
            error_string = sprintf('Test #%d should have not thrown an error but did',iTest);
            error(error_string);
        elseif ~strcmp(ME.identifier,error_id)
            ME
            error('Test: %d failed, but with the incorrect error',iTest);
        else
            fprintf('Test %d failed as expected\n',iTest);
            %fprintf('Test %d failed as expected with message:\n         %s\n',iTest,ME.message);
        end
        %TODO: Check identifier ...
        
    end
    
    if passed && ~should_pass
        error_string = sprintf('Test #%d should have thrown an error but didn''t',iTest);
        error(error_string);
    elseif passed
        expected_answer = tests{iTest,4};
        
        if ~isempty(expected_answer) && ~isequal(t,expected_answer)
            error('Test #%d failed because the parsed data did not match the expected value',iTest)
        else
            fprintf('Test %d passed as expected\n',iTest);
        end
    end
end