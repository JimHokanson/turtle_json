function input_checks()

js = '["this is a test"]';

%This is an incorrect usage, throw 'turtle_json:file_open'
t = json.fileToTokens(js);

js = '["this is a long test"';
js(end+1:205) = 'a';
js(end) = ']';
%This is an incorrect usage, throw 'turtle_json:file_open'
t = json.fileToTokens(js);

js = '["this is a test"]';
js(end+1:end+17) = [0 '\"' zeros(1,14)];
t = json.stringToTokens(js);
%t.mex.buffer_added should be 0
t = json.stringToTokens(uint8(js));

%This doesn't work, perhaps the conversion function is poor
js = '["this is a test"]';
js(end+1:end+17) = [0 '\"' zeros(1,14)];
js(end-5) = 'a'; 
t = json.stringToTokens(js);
%t.mex.buffer_added should be 1
t = json.stringToTokens(uint8(js));

t = json.stringToTokens(js);

end