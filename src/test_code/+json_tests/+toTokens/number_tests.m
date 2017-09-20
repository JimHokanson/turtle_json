function number_tests()
%

%{
    json_tests.toTokens.number_tests
%}

%Column 1: reset test number
%Column 2: json string
%Column 3: error identifier or expected value
%Column 4: Internal note as to what's happening


fh2 = @json_tests.utils.runTest;
fh2(1,'[1]','','single number in array',1);
%TODO: We could make this error more specific in the c code
fh2(0,'[+1]','turtle_json:invalid_token','''+'' is not ok to lead a number','');
fh2(0,'[1.]','turtle_json:no_fractional_numbers','''.'' needs to be followed by a numeric value','');
fh2(0,'[1.3]','','',1.3);
fh2(0,'[-1.3]','','',-1.3);

%TODO: This needs to be an error
fh2(0,'[1E]','','');

% tests(1,:) = {['[12312312,1231,12,' ...
%     '123123123123123123123123123123,123123]'],...
%     'turtle_json:integer_component_too_large','Too many digits in the number'};
% tests(end+1,:) = {'[1.]','turtle_json:no_fractional_numbers','Number with a period had no fractional numbers that followed'};
% tests(end+1,:) = {'[+1]','turtle_json:invalid_token','Can''t lead with a positive'};


% tests(3,:) = {'[1.3]',1.3,''};
% tests(4,:) = {'[-1.3]',-1.3,''};
% tests(5,:) = {'[-1.3e1]',-1.3e1,''};
% tests(6,:) = {'[-1.3E1]',-1.3E1,''};
% tests(7,:) = {'[-1.3E]',0,'exponent needs to be followed by a numeric value'};
% tests(8,:) = {'[5E3]',5E3,''};
% tests(9,:) = {'[5E+3]',5E+3,''};
% tests(10,:) = {'[5E-3]',5E-3,''};

%TODO: Throw in tests of numerical accuracy
