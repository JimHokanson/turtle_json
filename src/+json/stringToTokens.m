function obj = stringToTokens(input_string)

if ~ischar(input_string)
   error('First input must be a string')
end

obj = json.tokens(input_string,'raw_string',true);
end