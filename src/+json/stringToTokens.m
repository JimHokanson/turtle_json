function obj = stringToTokens(input_string)

if ~(ischar(input_string) || isa(input_string,'uint8') || isa(input_string,'int8'))
   error('First input must be a string')
end

obj = json.tokens(input_string,'raw_string',true);
end