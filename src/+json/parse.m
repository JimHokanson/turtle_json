function data = parse(json_string)
%x Return a fully parsed data structure from a string
%
%   data = json.parse(json_string)
%
%   Improvements
%   ------------
%   1) Pass in parsing options
%
%   See Also:
%   ---------
%   json.load  %json.load(file_path)
%   json.

tokens = json.stringToTokens(json_string);

data = tokens.getParsedData();

end