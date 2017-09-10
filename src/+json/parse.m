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
%   json.load

root = json.tokens.parse(json_string);

data = root.getParsedData();

end