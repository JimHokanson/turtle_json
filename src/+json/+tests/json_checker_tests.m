function json_checker_tests()
%
%   json.tests.json_checker_tests

t = cell(14,2);
t(1,:) = {'"A JSON payload should be an object or array, not a string."',0};
t(2,:) = {'["Unclosed array"',0};
t(3,:) = {'{unquoted_key: "keys must be quoted"}',0};
t(4,:) = {'["extra comma",]',0};
t(5,:) = {'["double extra comma",,]',0};
t(6,:) = {'[   , "<-- missing value"]',0};
t(7,:) = {'["Comma after the close"],',0};
t(8,:) = {'["Extra close"]]',0};
t(9,:) = {'"Extra comma": true,}',0};
t(10,:) = {'{"Extra value after close": true} "misplaced quoted value"',0};
t(11,:) = {'{"Illegal expression": 1 + 2}',0};
t(12,:) = {'{"Illegal invocation": alert()}',0};
t(13,:) = {'{"Numbers cannot have leading zeroes": 013}',0}; %??????
t(14,:) = {'{"Numbers cannot be hex": 0x14}',0};
% t(15,:) = 
% t(16,:) =
% t(17,:) = 
% t(18,:) = 
% t(19,:) =
% t(20,:) = 
% t(21,:) = 
% t(22,:) = 
% t(23,:) = 
% t(24,:) = 
% t(25,:) = 
% t(26,:) = 
% t(27,:) = 
% t(28,:) = 
% t(29,:) = 
% t(30,:) = 


n_tests = size(t,1);
for iTest = 1:n_tests
    cur_test_string = t{iTest,1};
    should_pass = t{iTest,2};
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

end