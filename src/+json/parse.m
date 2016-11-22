function data = parse(json_string)
%
%
%   data = json.parse(json_string)

tokens = json.stringToTokens(json_string);

data = tokens.get_parsed_data();

end