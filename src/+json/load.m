function data = load(file_path)
%
%   s = json.load(file_path);
%
%   

temp = json.fileToTokens(file_path);

data = temp.get_parsed_data();


end