function all_tests()
%
%   json_tests.all_tests();

fprintf('Running JSON checker tests\n');
json_tests.json_checker();

fprintf('Running "to token" tests\n');
json_tests.toTokens.all_token_tests();

fprintf('Running "to data" tests\n');
json_tests.toTokens.all_tests();

end