function input_options_testing()
%
%   json_tests.toTokens.input_options_testing

%{
    json_tests.toTokens.input_options_testing
%}

fh2 = @json_tests.utils.runTest;
encode = @json_tests.utils.encodeJSON;

%TODO: Missing passing a string into the file parser

js = '["this is a test"]';
js(end+1:end+17) = [0 '\"' zeros(1,14)];
%TODO: this doesn't work with the raw string, 
fh2(1,uint8(js),'','testing the buffer adding','',...
    @(x) x.root.mex.buffer_added == 0,'The buffer should not have been added');

%This doesn't work, something is not working with mxArrayToString
if false
fh2(0,js,'','testing the buffer adding','',...
    @(x) x.root.mex.buffer_added == 0,'The buffer should not have been added');
end



% % % t = json.tokens.parse(uint8(js));
% % % 
% % % fh2(1,'{"key":1,3}','turtle_json:no_key','3 should be a key, not a numeric','');
% % % fh2(0,'{:3}','turtle_json:invalid_token','Missing key');
% % % fh2(0,'{}','','empty object',struct); %empty object should be ok



% % % js = '["this is a test"]';
% % % 
% % % %This is an incorrect usage, throw 'turtle_json:file_open'
% % % t = json.fileToTokens(js);
% % % 
% % % js = '["this is a long test"';
% % % js(end+1:205) = 'a';
% % % js(end) = ']';
% % % %This is an incorrect usage, throw 'turtle_json:file_open'
% % % t = json.fileToTokens(js);


% % % % % t = json.tokens.parse(js);
% % % % % 
% % % % % 
% % % % % %assert('t.
% % % % % %t.mex.buffer_added should be 0
% % % % % 
% % % % % 
% % % % % %This doesn't work, perhaps the conversion function is poor
% % % % % js = '["this is a test"]';
% % % % % js(end+1:end+17) = [0 '\"' zeros(1,14)];
% % % % % 
% % % % % %We're introducing a mismatch here ...
% % % % % js(end-5) = 'a'; 
% % % % % tic; t = json.stringToTokens(js); toc;
% % % % % %t.mex.buffer_added should be 1
% % % % % tic; t = json.stringToTokens(uint8(js)); toc;
% % % % % 
% % % % % t = json.stringToTokens(js);

end