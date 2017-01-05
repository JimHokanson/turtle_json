function object_tests()
%
%   json_tests.toTokens.object_tests

%{
    json_tests.toTokens.object_tests
%}

%{
TODO: Move this somewhere else ...

s1 = struct('ab',1,'ac',2);
s2 = struct('ab',2,'ad',3);
js = mexEncodeJSON(temp);
s = json.stringToTokens(js);
    %check how many unique objects
%}

fh2 = @json_tests.utils.runTest;


fh2(1,'{"key":1,3}','turtle_json:no_key','3 should be a key, not a numeric','');
fh2(0,'{:3}','turtle_json:invalid_token','Missing key');
fh2(0,'{}','','empty object',struct); %empty object should be ok

%TODO: Build the struct inline
%Fails on the number?
s = struct;
s.a = 1;
fh2(0,'[{},{"a":1}]','','empty and non-empty objects',{struct,s});

end