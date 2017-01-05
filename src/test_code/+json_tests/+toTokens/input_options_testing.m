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
%assert('t.
%t.mex.buffer_added should be 0
t = json.stringToTokens(uint8(js));

%This doesn't work, perhaps the conversion function is poor
js = '["this is a test"]';
js(end+1:end+17) = [0 '\"' zeros(1,14)];

%We're introducing a mismatch here ...
js(end-5) = 'a'; 
tic; t = json.stringToTokens(js); toc;
%t.mex.buffer_added should be 1
tic; t = json.stringToTokens(uint8(js)); toc;

t = json.stringToTokens(js);

end