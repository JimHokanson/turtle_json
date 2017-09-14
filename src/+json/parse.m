function data = parse(json_string,varargin)
%x Return a fully parsed data structure from a string
%
%   data = json.parse(json_string,*data_options,*token_options);
%
%   data = json.parse(json_string,varargin);
%
%   Inputs
%   ------
%   json_string : uint8, int8, string
%   token_options : cell or empty
%       See json.load() for details
%   data_options : cell or empty
%       See json.load() for details
%
%   Examples
%   --------
%   JSON_STR = '[[1,2,3],[4,5,6]]';
%   data = json.parse(JSON_STR,'column_major',false);
%
%
%   See Also:
%   ---------
%   json.load
data_options = {};
token_options = {};
if nargin > 1
    if ischar(varargin{1})
        data_options = varargin;
    else
       data_options = varargin{1};
       if isempty(data_options)
           data_options = {};
       end
       if nargin > 2
           token_options = varargin{2};
           if isempty(token_options)
              token_options = {};
           end
       end
    end 
end

root = json.tokens.parse(json_string,token_options{:});

data = root.getParsedData(data_options{:});

end