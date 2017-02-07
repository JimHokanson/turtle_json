function data = load(file_path)
%
%   s = json.load(file_path);
%
%   TODO: Expose options

temp = json.tokens.load(file_path);

data = temp.getParsedData();


end