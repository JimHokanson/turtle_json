function all_tests()
%
%   
%   json_tests.toTokens.all_tests()
%{
    json_tests.toTokens.all_tests()
%}

fprintf('Running toTokens.array_tests()\n')
json_tests.toTokens.array_tests();

fprintf('Running toTokens.input_options_testing()\n');
json_tests.toTokens.input_options_testing();

%TODO: JSON checker tests

fprintf('Running toTokens.numeric_tests()\n')
json_tests.toTokens.number_tests();

fprintf('Running toTokens.object_tests()\n')
json_tests.toTokens.object_tests();

fprintf('Running toTokens.string_tests()\n')
json_tests.toTokens.string_tests();

end