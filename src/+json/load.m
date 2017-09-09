function data = load(file_path,token_options,convert_options)
%x Load json data from disk ...
%
%   s = json.load(file_path,varargin);
%
%   Main Options
%   ------------
%   
%
%   Other Options
%   -------------
%
%   See Also
%   --------
%   json.parse



%Retrieve tokens
temp = json.tokens.load(file_path,token_options{:});

%Parse the resulting data
data = temp.getParsedData();


end