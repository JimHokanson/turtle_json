function object_tests()
%
%   json_tests.toTokens.object_tests

%{
    json_tests.toTokens.object_tests
%}


fh2 = @json_tests.utils.runTest;
encode = @json_tests.utils.encodeJSON;

%Generic tests
%--------------------------------------------------------------------------
fh2(1,'{"key":1,3}','turtle_json:no_key','3 should be a key, not a numeric','');
fh2(0,'{:3}','turtle_json:invalid_token','Missing key');
fh2(0,'{}','','empty object',struct); %empty object should be ok

%TODO: Test a utf-8 encoded key ...

data = {struct,struct('a',1)};
fh2(0,encode(data),'','empty and non-empty objects',data);

%Tests on aspects of the parsing
%--------------------------------------------------------------------------
data = {struct('ab',1,'ac',2),struct('ab',2,'ad',3)};
fh2(0,encode(data),'','2 different structs',data,...
    @(x) x.root.mex.object_info.n_unique_objects == 2,'The # of unique objects should have been 2');
data = [struct('ab',1,'ac',2),struct('ab',2,'ac',3)];
fh2(0,encode(data),'','2 different structs',data,...
    @(x) x.root.mex.object_info.n_unique_objects == 1,'The # of unique objects should have been 1');
end